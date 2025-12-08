#!/bin/bash
# SafeC Pipeline Quick Commands Reference

# View workflow files location
echo "Workflow files are located at:"
echo "  .github/workflows/safec-analysis.yml"
echo "  .github/workflows/safec-quick-report.yml"
echo ""

# How to push changes
echo "To activate the pipeline, run:"
echo "  git add .github/"
echo "  git commit -m 'Add SafeC CI/CD pipeline workflows'"
echo "  git push origin main"
echo ""

# View pipeline status
echo "To view pipeline runs:"
echo "  1. Go to: https://github.com/<owner>/SafeC/actions"
echo "  2. Select the workflow you want to view"
echo "  3. Click on the run to see details"
echo ""

# Workflow triggers
echo "Workflows are triggered by:"
echo "  - Push to main branch"
echo "  - Push to develop branch"
echo "  - Pull requests to main branch"
echo "  - Pull requests to develop branch"
echo "  - Manual trigger (safec-quick-report.yml only)"
echo ""

# Manual trigger
echo "To manually trigger safec-quick-report.yml:"
echo "  1. Go to: https://github.com/<owner>/SafeC/actions"
echo "  2. Select: SafeC Analysis - Quick Report"
echo "  3. Click: Run workflow"
echo "  4. Select branch and click: Run workflow"
echo ""

# Download reports
echo "To download the HTML report:"
echo "  1. Go to: https://github.com/<owner>/SafeC/actions"
echo "  2. Click on the completed workflow run"
echo "  3. Scroll to: Artifacts section"
echo "  4. Download: safec-analysis-<run-id>.zip"
echo "  5. Extract and open JulietPipelineReport.html in browser"
echo ""

# Monitor PR analysis
echo "For pull requests:"
echo "  - Workflow automatically runs"
echo "  - Comment posted with report link"
echo "  - Report available in Artifacts for 30-90 days"
echo ""

# Customize analysis target
echo "To analyze different directory:"
echo "  Edit .github/workflows/safec-quick-report.yml"
echo "  Change line:"
echo "    ./build/safec -d --html JulietPipelineReport.html ./your-directory"
echo ""

# View workflow logs
echo "To view detailed logs:"
echo "  1. Click on workflow run in Actions tab"
echo "  2. Click on the step you want to inspect"
echo "  3. Logs will expand"
echo ""

# Common issues
echo "Troubleshooting:"
echo "  - Workflow not running? → Check Files are committed"
echo "  - Build fails? → Check CMakeLists.txt and build logs"
echo "  - No report? → Verify Juliet submodule is cloned"
echo "  - Want logs? → Download analysis.log from artifacts"
echo ""
