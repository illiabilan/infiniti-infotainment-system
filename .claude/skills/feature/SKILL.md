---
name: feature
description: Runs the full feature workflow for this repo. Syncs main, creates a feature branch, researches (web, code, git), writes an implementation plan with the feature-architect agent, runs a developer and tester loop (max 4 iterations), runs a reviewer gate, then commits, pushes, and opens a pull request. Use only when the user explicitly asks to build a feature end to end.
argument-hint: "<feature description>"
disable-model-invocation: true
---

# Feature Workflow

Input: `$ARGUMENTS` is the feature description. If it is empty, ask the user
for one and stop.

## Naming

- `feature-name`: kebab-case slug of the description, max 40 characters.
- Branch: `feature-<feature-name>`.
- Work folder: `.personal/feature-<feature-name>/`. The folder name matches the branch name.
- Files in the work folder: `research.md`, `implementation-plan.md`, `to-fix.md`, `changelog.md`.

Branch names use `feature-` instead of `feature/` so the folder name matches the branch name without nesting.

## Step 0: Preconditions

Run `git status --porcelain`. If any tracked file shows as modified (`M`, `A`, `D`, `R`), stop and ask the user to commit or stash it. Untracked files are allowed.

If the branch `feature-<feature-name>` already exists locally or on `origin`, stop and ask the user.

## Step 1: Sync main and create the branch

```
git checkout main
git pull --rebase origin main
git checkout -b feature-<feature-name>
mkdir -p .personal/feature-<feature-name>
```

## Step 2: Research

Launch three `researcher` agents in one message, each with `effort: "high"`. Do not override the model. The agent runs on its own defined model, haiku. Each prompt includes the feature description, the instruction to read `AGENTS.md`, and the instruction to return its report without writing files.

| Agent | Scope | Tools it should use |
|---|---|---|
| web | External facts the feature depends on (Qt APIs, standards, library behavior) | WebSearch, WebFetch, and the `qt-docs` MCP tools for Qt APIs |
| code | Existing code the feature touches or reuses | Grep, Glob, Read |
| git | History of the affected files and conventions | Bash with read-only git (`log`, `show`, `blame`) |

Web is optional. If the feature needs no external facts, skip that agent and write `Web: skipped - <reason>` in `research.md`.

Write the three reports to `.personal/feature-<feature-name>/research.md`, with sections `## Web`, `## Code`, `## Git`, and `## Gaps`.

## Step 3: Implementation plan

Launch one `feature-architect` agent with `effort: "xhigh"`. Its prompt contains:

1. The feature description as the ticket task.
2. The full contents of `research.md`, inline.
3. An instruction to load the `qt-qml`, `qt-cmake-project`, and `qt-ui-design` skills with the Skill tool and apply them to the architecture decisions.

If the output is a "Needs Decision" report, stop and show it to the user. Do not continue.

Otherwise, write the `PLAN_CONTENT` block to `.personal/feature-<feature-name>/implementation-plan.md`.

## Step 4: Developer and tester loop

Maximum 4 iterations per entry into this loop. Each entry starts a fresh count. The loop is entered once from Step 3 and again after each reviewer failure in Step 5.

For iteration `i`:

1. **Developer.** Launch `developer` with `effort: "high"`. The prompt gives the path to `implementation-plan.md`. If `to-fix.md` exists, the prompt also gives its path and says to fix every item in it. If the developer reports Blockers, stop and show them to the user.
2. **Tester.** Launch `tester` with `effort: "high"`. The prompt says to run the project's own validation and return the report. The tester does not write files.
3. **Save the result.** The main session writes the file. The tester stays read-only.
   - Overall Verdict `PASS`: write `changelog.md` with the developer's summary and Files Changed, plus the tester's Results. Delete `to-fix.md` if it exists. Go to Step 5.
   - Overall Verdict `FAIL`: write `to-fix.md` with the tester's failing checks, with their evidence and file:line references. If iterations remain, repeat from 1. If not, stop and report the remaining failures to the user.
   - Overall Verdict `INCONCLUSIVE`: stop and ask the user. Blocked checks need an environment fix, which a loop cannot provide.

## Step 5: Reviewer gate

Launch `reviewer` with `effort: "high"`. The prompt gives the paths to `implementation-plan.md`, `research.md`, and `changelog.md`, and says to review the working tree against `main`, including untracked files listed by `git status --porcelain`. The prompt also names `.personal/feature-<feature-name>/review.md` as the output path. The reviewer writes its report there with Write, overwriting the file on each attempt. Confirm the file exists before reading the verdict.

Decide the result from the report:

- **Pass:** zero Blocking and zero Major findings. Go to Step 6.
- **Fail:** any Blocking or Major finding. Write those findings to `to-fix.md`, then return to Step 4 with a fresh 4-iteration budget. The reviewer gate allows maximum 4 review attempts. Count every reviewer run. After the 4th failed review, stop and report the findings to the user.

## Step 6: Handoff

1. Stage everything: `git add -A`. The user asked for everything, so this includes `.personal/` and any untracked files that were already in the working tree (for example `docs/` and `.claude/`). Show `git status --short` to the user in the final message.
2. Commit using the Conventional Commits format from the repo guidelines:
   - Header: lowercase type, imperative, under 72 characters, no trailing period.
   - One blank line, then bullets for the architecture, feature, and configuration changes.
   - No `Co-Authored-By` trailer.
3. Push: `git push -u origin feature-<feature-name>`.
4. Write the PR body to the scratchpad directory, not the repo. Keep it at 400 words or fewer, with these sections: Summary, Changes, Verification (the tester's results), Review (the reviewer's verdict), Notes. End it with `🤖 Generated with [Claude Code](https://claude.com/claude-code)`. Check the count with `wc -w`.
5. Create the PR: `gh pr create --base main --head feature-<feature-name> --title "<conventional header>" --body-file <scratchpad path>`.
6. Report the PR URL.

## Rules

- Never claim a build, test, or review passed without the agent's report as evidence.
- Each agent runs once per iteration. Do not re-run a passing agent.
- Keep the work folder as the only source of truth between steps. Agents do not share context.
