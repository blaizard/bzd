import argparse
import pathlib
import typing
import json
import time
import random

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Create diagram explorer in markdown.")
	parser.add_argument("bdls", type=pathlib.Path, nargs="+", help="bdls in json format.")

	args = parser.parse_args()

	uid = f"{time.time()}.{random.random()}".replace(".", "")
	content = """
<script>
	let bdlToSvg = null;
	const {redrawFctName} = async (selectObject) => {{
		const config = (selectObject)? selectObject.value : "apps";
		await loadScript("$(path /external/bzd_bdl+/diagram/library.output/library.umd.cjs)");
		const bdls = [{bdlsJson}];
		if (bdlToSvg === null) {{
			bdlToSvg = new BdlToSvg(bdls);
		}}
		const svg = await bdlToSvg.render(config == "all");
		document.getElementById("{divId}").innerHTML = svg;
	}};
	window.onload = {redrawFctName};
</script>
<select onchange="{redrawFctName}(this)">
  <option value="all">All</option>
  <option value="apps" selected>Apps</option>
</select>
<div id="{divId}">Diagram Loading...</div>
""".format(bdlsJson=",".join([bdl.read_text() for bdl in args.bdls]),
	       divId=f"diagram_{uid}",
	       redrawFctName=f"redraw_{uid}")
	print(content)
