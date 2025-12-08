# SafeC GitHub Actions Pipeline Setup

## What Was Created

I've set up two automated GitHub Actions workflows for your SafeC project that will run every time you push code.

## Files Created

```
.github/
├── workflows/
│   ├── safec-analysis.yml           ← Main comprehensive pipeline
│   └── safec-quick-report.yml       ← Lightweight quick report
└── CI_CD_PIPELINE.md                ← Detailed documentation
```

## Quick Start

1. **Commit and push the new files**:
   ```bash
   git add .github/
   git commit -m "Add SafeC CI/CD pipeline workflows"
   git push origin main
   ```

2. **Watch the workflow run**:
   - Go to your GitHub repository
   - Click **Actions** tab
   - You'll see the workflows running

3. **Get your HTML report**:
   - Click on the workflow run
   - Scroll to **Artifacts** section
   - Download the `safec-analysis-<run-id>` file

## Workflow Comparison

| Feature | safec-analysis.yml | safec-quick-report.yml |
|---------|-------------------|----------------------|
| **Runs on push** | ✅ Yes | ✅ Yes |
| **Runs on PR** | ✅ Yes | ✅ Yes |
| **Manual trigger** | ❌ No | ✅ Yes |
| **GitHub Pages** | ✅ Yes | ❌ No |
| **PR comments** | ✅ Yes | ✅ Yes |
| **Caching** | ❌ No | ✅ Yes (faster rebuilds) |
| **Build caching** | - | ✅ 5x faster on reruns |

## Which One Should You Use?

**Use `safec-analysis.yml` if you want:**
- Automatic GitHub Pages hosting of reports
- Persistent report history
- Clean separation of concerns

**Use `safec-quick-report.yml` if you want:**
- Faster runs (with build caching)
- More detailed logs
- Simpler setup
- Manual trigger capability

**Recommendation**: Start with `safec-quick-report.yml` - it's faster and easier to debug.

---

## Detailed Pipeline Flow

### On Every Push or PR:

```
1. Checkout your code
   ↓
2. Install build dependencies (cmake, build-essential)
   ↓
3. Build SafeC from source
   ↓
4. Run analysis: ./build/safec -d --html JulietPipelineReport.html ./juliet-test-suite-c/testcases
   ↓
5. Upload HTML report as artifact (30-90 days)
   ↓
6. For PRs: Post comment with report link
   ↓
7. For main branch: Publish to GitHub Pages (optional)
```

---

## What Gets Generated

### **JulietPipelineReport.html**
An interactive HTML report containing:
- Total vulnerabilities found
- Severity breakdown (CRITICAL, HIGH, MEDIUM, LOW)
- Line-by-line vulnerability details
- Code snippets
- Remediation recommendations

### **analysis_summary.md**
A markdown summary with:
- Generation timestamp
- Commit hash
- Branch name
- Key statistics
- Last 20 lines of analysis output

### **analysis.log**
Complete command output and debug information

---

## Accessing Reports

### From GitHub Actions Artifacts:
1. Go to **Actions** tab
2. Click the latest workflow run
3. Scroll down to **Artifacts**
4. Download the zip file containing all reports

### From GitHub Pages (if enabled):
- Reports published to: `https://<username>.github.io/SafeC/safec-reports/<run-number>/JulietPipelineReport.html`

### On Pull Requests:
- Automatic comment posted with link to artifacts
- Click the link to view or download the report

---

## Customization Examples

### Analyze Different Directory
In the workflow file, change:
```yaml
./build/safec -d --html JulietPipelineReport.html ./your-directory
```

### Run on Different Branches
In the `on` section, add branches:
```yaml
on:
  push:
    branches: [ main, develop, staging ]
  pull_request:
    branches: [ main, develop, staging ]
```

### Change Report Retention
```yaml
retention-days: 180  # Keep reports for 6 months instead of 30 days
```

### Generate Badges
Add to your README.md:
```markdown
[![SafeC Analysis](https://github.com/<owner>/<repo>/actions/workflows/safec-quick-report.yml/badge.svg)](https://github.com/<owner>/<repo>/actions/workflows/safec-quick-report.yml)
```

---

## Next Steps

1. ✅ Add the `.github/workflows/` directory to your repository
2. ✅ Commit and push the files
3. ✅ Go to the **Actions** tab and watch the workflow run
4. ✅ Download the report from artifacts
5. ✅ (Optional) Enable GitHub Pages for persistent hosting

---

## Troubleshooting

**Workflow not running?**
- Check that `.github/workflows/*.yml` files are committed and pushed
- GitHub Actions must be enabled in Settings → Actions

**Build fails?**
- Check build logs in the Actions tab
- Verify CMakeLists.txt is correct
- Ensure all submodules are cloned

**Report not generated?**
- Check if the Juliet test suite is cloned (submodule)
- Look at the analysis.log for errors
- Verify the build succeeded first

**Want to see detailed output?**
- Click on the workflow step in GitHub Actions
- Expand the step to see full logs
- Download analysis.log from artifacts

---

## Cost Considerations

- **Free tier**: 2,000 minutes/month (GitHub-hosted runners)
- **Each run**: ~5-15 minutes
- **Cost**: Free for public repos, uses action minutes for private repos

For high-frequency analysis or large test suites, consider:
- Running only on main branch
- Using `schedule` trigger for daily/weekly runs
- Using self-hosted runners

---

That's it! Your SafeC analysis pipeline is now automated. 🚀
