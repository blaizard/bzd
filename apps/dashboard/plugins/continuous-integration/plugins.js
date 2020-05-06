"use strict";

export default {
    visualization: {
        name: "Continuous Integration",
        icon: "bzd-icon-continuous-integration",
        form: [],
        frontend: () => import("./visualization.vue")
    }
};
