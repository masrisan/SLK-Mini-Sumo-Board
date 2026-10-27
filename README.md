# masrisanTekNo — SLK Mini Sumo Board

Static product website designed for GitHub Pages. No build process or dependencies are required.

## Contents
- `index.html` — homepage, product overview, technical specifications, downloads and wiring setup
- `assets/` — product photographs and wiring diagram
- `downloads/` — firmware sketches and the SLK tuning guide PDF
- `.nojekyll` — disables Jekyll processing for this static site

## Publish with GitHub Pages
1. Create a repository, e.g. `masrisantekno`.
2. Upload the **contents** of this folder to the repository root.
3. Open **Settings → Pages**.
4. Under Build and deployment, select **Deploy from a branch**.
5. Select `main` and `/(root)`, then save.
6. Wait for GitHub Pages to finish deployment; the site URL will appear on the Pages settings page.

## Before publishing
- Check that the wiring diagram matches the PCB revision you intend to document.
- Confirm electrical current and voltage ratings before publishing them.
- Add contact details when ready.
- Firmware files are provided as uploaded; review and test them before distributing.

## Updating downloads
Put new downloadable files in `downloads/` and add a matching link in `index.html`.
