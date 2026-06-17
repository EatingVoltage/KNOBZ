# Grill: consolidate to a single public GitHub repo
Date: 2026-06-17

## Intent
Retire the old Bitbucket repo (`the-little-fader-bank`, the pre-rename TLFB name) and
make `github.com/EatingVoltage/KNOBZ` the single source of truth: public, GPL-3.0,
holding the current code and real development history.

## Constraints
- Cannot delete the Bitbucket repo from here — no Bitbucket credentials (gh auth is
  GitHub-only, account EatingVoltage). User must delete it manually in Bitbucket.
- GitHub `EatingVoltage/KNOBZ` already exists: public, GPL-3.0, default branch `main`,
  but holds a STALE, unrelated TLFB-era snapshot (commit 7d90324, pushed 2026-05-06),
  including a `tinylittleFaderBank.code-workspace` file.
- Local repo is the real current code: tip 72c3777 (today's lidar-switch commit), but on
  a DETACHED HEAD, ahead of local `master`/`dev`, with NO LICENSE file, remote = Bitbucket.

## Key decisions
- Decision: Force-push local 43-commit history onto GitHub `main`, discarding GitHub's
  stale unrelated commit (GPL LICENSE re-added into the tree first so nothing is lost).
  Reason: local history is the project's real trail; the GitHub commit is a throwaway.
  Alternatives rejected: (b) squash to one fresh commit (loses dev trail);
  (c) graft histories (fragile, unrelated roots, no benefit).
- Decision: `main` only. Collapse detached HEAD onto `main` at 72c3777; delete local
  `master`/`dev` and the stale remote `master`. Reason: solo embedded project, `dev` was
  stale/behind, "one repo" simplicity. Alternative rejected: keep `main` + `dev`.
- Decision: Drop the Bitbucket `origin` remote entirely; repoint `origin` to GitHub.
  Reason: user wants Bitbucket fully gone, not parked. Alternative rejected: park it as a
  `bitbucket` remote temporarily as rollback.
- Decision: Keep GPL-3.0 (the "current licensing"); bring GitHub's GPL LICENSE into the
  local tree. Reason: matches existing GitHub license; user wants licensing preserved.
- Decision: Rename `tinylittleFaderBank.code-workspace` -> `KNOBZ.code-workspace`.
  Reason: finishes the TLFB->KNOBZ rename; it was the last TLFB-named artifact in the tree.

## Surfaced assumptions
- "Current licensing" = the GPL-3.0 already on the GitHub repo (local tree had none).
- The two repos are genuinely separate histories (different roots), not a fork/clone.
- The `.grill/*.md` design logs are already tracked and will become public on GitHub;
  left as-is per "go full / public" (flagged to user).

## Out of scope
- Deleting the Bitbucket repo (user does it manually).
- Rewriting commit messages / cleaning the `commit`/`recommit` noise in history.
- Touching `.travis.yml` (dead CI config left untouched; not part of the ask).
- Writing a real README (GitHub's is a 24-byte stub; not requested).
