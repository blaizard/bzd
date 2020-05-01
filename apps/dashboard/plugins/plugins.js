"use strict";

import Jenkins from "./jenkins/plugins.js"
import ContinuousIntegration from "./continuous-integration/plugins.js"

export const Source = {
    "jenkins": Jenkins.source
}

export const Visualization = {
    "continuous-integration": ContinuousIntegration.visualization
}
