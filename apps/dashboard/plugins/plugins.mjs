"use strict";

import Jenkins from "./jenkins/plugins.mjs"
import TravisCI from "./travisci/plugins.mjs"
import Github from "./github/plugins.mjs"
import ContinuousIntegration from "./continuous-integration/plugins.mjs"
import Link from "./link/plugins.mjs"

export const Source = {
    "jenkins": Jenkins.source,
    "travisci": TravisCI.source,
    "github": Github.source
}

export const Visualization = {
    "continuous-integration": ContinuousIntegration.visualization,
    "link": Link.visualization
}
