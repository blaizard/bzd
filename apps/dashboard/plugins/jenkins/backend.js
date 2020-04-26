"use strict";

import Fetch from "../../../../nodejs/core/fetch/fetch.js";

export default {
    fetch: async (data) => {

        await Fetch.get("/configuration");

        return {

            

        };
    }
}
