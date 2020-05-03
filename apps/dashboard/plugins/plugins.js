"use strict";

import Jenkins from "./jenkins/plugins.js"
import TravisCI from "./travisci/plugins.js"
import ContinuousIntegration from "./continuous-integration/plugins.js"

export const Source = {
    "jenkins": Jenkins.source,
    "travisci": TravisCI.source
}

export const Visualization = {
    "continuous-integration": ContinuousIntegration.visualization
}
