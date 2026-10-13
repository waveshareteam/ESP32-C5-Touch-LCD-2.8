# CI and local checks

English | [简体中文](ci_ZH.md) | [Home](../README.md)

[Example checks](../.github/workflows/examples.yml) runs a lightweight documentation/selection job on every pull request and push to main. The final aggregate check remains visible even when example builds are skipped.

- Markdown changes at the root, inside examples, or inside bundled libraries select no example builds.
- A source/configuration change inside one example selects that example.
- Shared IDF components select all IDF projects; bundled Arduino library changes select all sketches.
- Workflow, CI script, test or policy changes select all 22 configured builds.
- Factory firmware paths are classified separately. Changes to their source or artifacts do not select example builds.
- Missing/empty diff data fails selection. Manual dispatch accepts `all`, an exact example name, or its repository-relative path.
- A newer run cancels an obsolete run for the same pull request or branch.

The full matrix contains 8 projects × 2 IDF versions and 6 Arduino sketches. Source-built artifacts are not published as factory firmware.

## Local validation

From the repository root:

```sh
python -m unittest discover -s tests -v
python scripts/ci_plan.py --working-tree
python scripts/ci_plan.py --dispatch all
```

The working-tree command runs both full audit tools with the repository's ownership/routing policies and writes `.ci-output/plan.json`. For documentation-only changes it also enforces `--expect-docs-only` and `--expect-no-example-builds`. The dispatch command audits all maintained Markdown and selects the requested examples.

CI invokes `python scripts/ci_plan.py --event`; integration tests exercise this exact event entry point and its emitted matrices. Audit scripts also have synthetic coverage for ownership, language pairing, links/fragments, homepage structure, privacy, firmware boundaries and incomplete diffs.

See [build commands](../README.md) and [validation status](validation.md). Generated build output, dependency downloads and local editor configuration are ignored.

## Document ownership

English pages have adjacent `_ZH.md` companions. The former `README.en.md` and `README.cn.md` are paired compatibility entries. The single pull-request template is bilingual because GitHub consumes one default path.

Bundled libraries, GUI Guider resources, Brookesia upstream docs and SHTC3 upstream docs retain their ownership and filenames. Local BSP/driver maintenance notes live in [compatibility](compatibility.md).
