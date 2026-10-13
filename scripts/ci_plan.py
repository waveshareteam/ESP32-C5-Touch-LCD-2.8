#!/usr/bin/env python3
"""Validate documentation and select example matrices from complete GitHub events."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

from audit_ci_routing import RoutingError, load_routing_config, route_changes
from audit_markdown import (
    AuditError, Change, changes_from_base, changes_from_worktree, load_config,
)
from inventory_repo import make_inventory


def select_paths(available: list[str], selector: str) -> list[str]:
    if selector == "all":
        return available
    wanted = selector.strip("/")
    selected = [
        path for path in available
        if wanted in {path, str(Path(path).parent).replace("\\", "/"), Path(path).stem, Path(path).name}
    ]
    if len(selected) != 1:
        raise ValueError("Select 'all', one exact example name, or one example path.")
    return selected


def make_plan(root: Path, changes: list[Change] | None, selector: str = "all") -> dict:
    policy = load_config(root / "config/markdown-audit.json")
    if changes is None:
        inventory = make_inventory(root, max_files=50000, max_text_files=20000, policy_config=policy)
        if inventory["scan_truncated"]:
            raise ValueError("Inventory is incomplete.")
        available = inventory["example_ci_idf_projects"] + inventory["arduino_sketches"]
        selected = select_paths(available, selector)
        idf = [p for p in selected if p in inventory["example_ci_idf_projects"]]
        arduino = [p for p in selected if p in inventory["arduino_sketches"]]
        docs_only = False
        report = {"dispatch": selector, "esp_idf": idf, "arduino": arduino}
    else:
        report = route_changes(
            root, changes, load_routing_config(root / "config/ci-routing.json"),
            policy, max_files=50000, max_text_files=20000,
        )
        idf = report["esp_idf"]["selected"]
        arduino = report["arduino"]["selected"]
        docs_only = report["scope"]["docs_only"]
    versions = json.loads((root / "config/ci.json").read_text(encoding="utf-8"))
    return {
        "idf": [{"path": p, "name": Path(p).name, "version": v, "target": versions["target"]}
                for p in idf for v in versions["idf_versions"]],
        "arduino": [{"path": p, "name": Path(p).stem, "core": versions["arduino_core"],
                     "fqbn": versions["arduino_fqbn"]} for p in arduino],
        "docs_only": docs_only,
        "routing": report,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[1])
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--event", action="store_true")
    mode.add_argument("--working-tree", action="store_true")
    mode.add_argument("--dispatch", metavar="NAME_OR_PATH")
    args = parser.parse_args()
    root = args.repo.resolve()
    selector = args.dispatch or "all"
    source: list[str]
    if args.working_tree:
        changes = changes_from_worktree(root)
        source = ["--working-tree"]
    elif args.dispatch:
        changes = None
        source = ["--all"]
    else:
        event = json.loads(Path(os.environ["GITHUB_EVENT_PATH"]).read_text(encoding="utf-8"))
        event_name = os.environ["GITHUB_EVENT_NAME"]
        if event_name == "workflow_dispatch":
            changes = None
            selector = event.get("inputs", {}).get("example") or "all"
            source = ["--all"]
        elif event_name in {"pull_request", "push"}:
            base = event["pull_request"]["base"]["sha"] if event_name == "pull_request" else event["before"]
            if not base or set(base) == {"0"}:
                raise ValueError("No usable base SHA; run workflow_dispatch to validate this branch.")
            changes = changes_from_base(root, base)
            source = ["--base", base]
        else:
            raise ValueError("Unsupported workflow event.")
    plan = make_plan(root, changes, selector)
    audit_cmd = [
        sys.executable, str(root / "scripts/audit_markdown.py"), str(root),
        *source, "--config", str(root / "config/markdown-audit.json"),
    ]
    if plan["docs_only"]:
        audit_cmd += ["--expect-docs-only"]
    subprocess.run(audit_cmd, check=True)
    if changes is not None:
        routing_cmd = [
            sys.executable, str(root / "scripts/audit_ci_routing.py"), str(root),
            *source, "--ownership-config", str(root / "config/markdown-audit.json"),
            "--routing-config", str(root / "config/ci-routing.json"), "--max-text-files", "20000",
        ]
        if plan["docs_only"]:
            routing_cmd += ["--expect-docs-only", "--expect-no-example-builds"]
        subprocess.run(routing_cmd, check=True, stdout=subprocess.DEVNULL)
    output = root / ".ci-output"
    output.mkdir(exist_ok=True)
    (output / "plan.json").write_text(json.dumps(plan, indent=2), encoding="utf-8")
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as stream:
            for key in ("idf", "arduino"):
                stream.write(f"{key}={json.dumps(plan[key], separators=(',', ':'))}\n")
    print(f"Selected {len(plan['idf'])} ESP-IDF builds and {len(plan['arduino'])} Arduino builds.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AuditError, RoutingError, ValueError, KeyError, OSError, subprocess.CalledProcessError) as exc:
        print(f"CI planning failed: {exc}", file=sys.stderr)
        raise SystemExit(2)
