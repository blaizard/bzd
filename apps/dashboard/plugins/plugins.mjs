

import Jenkins from "./jenkins/plugins.mjs";
import TravisCI from "./travisci/plugins.mjs";
import Github from "./github/plugins.mjs";
import Coverage from "./coverage/plugins.mjs";
import ContinuousIntegration from "./continuous_integration/plugins.mjs";
import Link from "./link/plugins.mjs";

export const Source = {
	"jenkins": Jenkins.source,
	"travisci": TravisCI.source,
	"github": Github.source,
	"coverage": Coverage.source
};

export const Visualization = {
	"continuous-integration": ContinuousIntegration.visualization,
	"link": Link.visualization,
	"coverage": Coverage.visualization
};
