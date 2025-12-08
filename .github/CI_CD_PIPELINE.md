# SafeC CI/CD Pipeline Documentation

This document explains the automated GitHub Actions pipelines for SafeC static analysis.

## Overview

Two GitHub Actions workflows are configured to automatically run SafeC analysis:

1. **safec-analysis.yml** - Full pipeline with GitHub Pages publishing
2. **safec-quick-report.yml** - Quick report with PR comments

## Workflow 1: Full Analysis Pipeline (`safec-analysis.yml`)

### Triggers
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches

### What it does
1. **Checkout Code** - Clones repository with submodules
2. **Install Dependencies** - Installs build-essential, cmake, git
3. **Build SafeC** - Compiles the analyzer using CMake
4. **Run Analysis** - Executes SafeC on the Juliet test suite
   ```bash
   ./build/safec -d --html JulietPipelineReport.html ./juliet-test-suite-c/testcases
   ```
5. **Upload Artifacts** - Stores HTML report for 30 days
6. **Publish to GitHub Pages** - (Main branch pushes only)
   - Reports available at: `https://<username>.github.io/SafeC/safec-reports/<run-number>/`
7. **PR Comment** - Adds comment with report link on pull requests

### Artifacts
- `JulietPipelineReport.html` - Full interactive analysis report
- Available in: Actions → Workflow Run → Artifacts

## Workflow 2: Quick Report (`safec-quick-report.yml`)

### Triggers
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches
- Manual trigger (workflow_dispatch)

### What it does
1. **Checkout Code** - Clones repository with submodules
2. **Cache Build** - Caches compiled binaries for faster runs
3. **Install Dependencies** - Minimal system dependencies
4. **Build SafeC** - Release build for optimal performance
5. **Run Analysis** - Full test suite analysis
6. **Generate Summary** - Creates markdown summary with logs
7. **Upload Artifacts** - Stores report, summary, and logs (90 days)
8. **Post PR Comment** - Comments on PRs with summary and report link

### Output Files
- `JulietPipelineReport.html` - Full HTML report
- `analysis_summary.md` - Markdown summary
- `analysis.log` - Full command output

## File Structure

```
.github/
├── workflows/
│   ├── safec-analysis.yml          # Full pipeline
│   └── safec-quick-report.yml      # Quick report
└── README.md                        # This file
```

## GitHub Pages Setup (Optional)

To enable automatic GitHub Pages publishing:

1. Go to **Settings** → **Pages**
2. Set **Source** to `gh-pages` branch
3. Save

Reports will be published to: `https://<username>.github.io/SafeC/safec-reports/`

## Environment Variables

No special environment variables are required. The workflows use:
- `GITHUB_TOKEN` - Automatically provided by GitHub Actions
- Standard GitHub context variables (`github.sha`, `github.ref_name`, etc.)

## Monitoring & Results

### View Artifacts
1. Go to **Actions** tab
2. Click on the workflow run
3. Scroll to **Artifacts** section
4. Download `safec-analysis-<run-id>` zip file

### PR Comments
- Comments appear automatically on pull requests
- Include link to full HTML report in artifacts
- Show tail of analysis output

### Analysis Reports
- Full HTML interactive reports with vulnerability details
- Line-by-line code references
- Severity classifications (LOW, MEDIUM, HIGH, CRITICAL)
- Remediation recommendations

## Manual Workflow Trigger

To manually trigger the quick report workflow:

1. Go to **Actions** tab
2. Select **SafeC Analysis - Quick Report**
3. Click **Run workflow**
4. Select branch and click **Run workflow**

## Performance Notes

- **Build Time**: ~1-2 minutes
- **Analysis Time**: Depends on test suite size, typically 2-10 minutes
- **Total Time**: ~5-15 minutes per run

## Customization

### Change Test Suite
Edit the analysis command in either workflow file:
```yaml
./build/safec -d --html JulietPipelineReport.html ./juliet-test-suite-c/testcases
```

To analyze a specific directory:
```yaml
./build/safec -d --html JulietPipelineReport.html ./your-code-directory
```

### Change Report Name
Replace `JulietPipelineReport.html` with your preferred filename in both:
1. Workflow file (analysis step)
2. Upload artifacts step

### Change Artifact Retention
Modify `retention-days` in upload-artifact step:
```yaml
retention-days: 90  # Change this number
```

### Change Branch Triggers
Edit the `on` section:
```yaml
on:
  push:
    branches: [ main, develop, staging ]  # Add/remove branches
  pull_request:
    branches: [ main, develop, staging ]
```

## Troubleshooting

### Build Fails
- Check CMakeLists.txt for errors
- Verify all dependencies are listed in apt-get install
- Check build logs in Actions → Workflow Run

### Analysis Takes Too Long
- Reduce test suite size
- Analyze specific subdirectories instead
- Increase Ubuntu runner machine type (in workflow file)

### Report Not Generated
- Check if build succeeded first
- Verify Juliet test suite is cloned (check submodules)
- Check analysis.log for errors

### GitHub Pages Not Publishing
- Ensure gh-pages branch exists or allow workflow to create it
- Check Pages settings in repository settings
- Verify GITHUB_TOKEN has write permissions

## Security Considerations

- Reports contain vulnerability information - keep private or use private GitHub Pages
- Consider using branch protection rules to review reports before merging
- Store reports long-term in artifact storage or external service

## Integration with Other Tools

### Send Results Elsewhere
Add a step to push results to:
- Slack webhook
- Email notification
- External database
- Static site generator

Example for Slack:
```yaml
- name: Notify Slack
  uses: slackapi/slack-github-action@v1
  with:
    webhook-url: ${{ secrets.SLACK_WEBHOOK }}
    payload: |
      text: "SafeC Analysis Complete"
      blocks:
        - type: "section"
          text:
            type: "mrkdwn"
            text: "Run: ${{ github.run_id }}\nBranch: ${{ github.ref_name }}"
```

## Support

For issues or questions:
1. Check workflow logs in GitHub Actions
2. Review SafeC documentation
3. Check Juliet test suite documentation

---

**Last Updated**: December 9, 2025
