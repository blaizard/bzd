export default {
  name: "Docker Registry",
  icon: "bzd-icon-link",
  form:
	  [
		{
		  type : "Dropdown",
		  name : "docker.type",
		  caption : "Registry Type",
		  list : {
			v2 : "Docker V2",
			gcr : "Google Container Registry",
		  },
		  validation : "mandatory",
		},
		{type : "Input", name : "docker.url", caption : "Registry URL", condition : {"docker.type" : "values(v2)"}},
		{
		  type : "Dropdown",
		  name : "docker.service",
		  editable : true,
		  caption : "Service",
		  list : [ "gcr.io", "us.gcr.io", "eu.gcr.io", "asia.gcr.io" ],
		  validation : "mandatory",
		  condition : {"docker.type" : "values(gcr)"},
		},
		{
		  type : "Textarea",
		  name : "docker.key",
		  caption : "JSON key",
		  validation : "mandatory",
		  condition : {"docker.type" : "values(gcr)"},
		},
		{type : "Checkbox", name : "docker.proxy", caption : "Proxy"},
		{
		  type : "Input",
		  name : "docker.proxy.url",
		  caption : "Proxy Registry URL",
		  width : 0.5,
		  validation : "mandatory",
		  placeholder : "https://docker.blaizard.com",
		  condition : (v) => v["docker.proxy"],
		},
		{
		  type : "Input",
		  name : "docker.proxy.port",
		  caption : "Proxy Registry Port",
		  width : 0.5,
		  validation : "mandatory type(integer)",
		  placeholder : 8000,
		  condition : (v) => v["docker.proxy"],
		},
	  ],
  view: () => import("../default.vue"),
};
