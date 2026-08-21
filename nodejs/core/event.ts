/// Add events functionality
export default class Event {
    options: Record<string, any> = {};
    proactive: Record<string, any> = {};
    list: Record<string, any> = {};

    constructor(options?: Record<string, any>) {
        // Loop through event options and adjust them
        const opts = options || {};
        for (const id in opts) {
            this.options[id] = Object.assign(
                {
                    /// Set to true if the event once triggered will also
                    /// trigger a new one.
                    proactive: false,
                },
                opts[id],
            );
        }
        this.clear();
    }

    /// \brief Register an event
    ///
    /// \param id The identifier of the event
    /// \param callback The function to be called when the event is triggered
    /// \param once If the event is intended to be fired only once
    on(id: string, callback: (...args: any[]) => void, once: boolean = false): Event {
        let addToList = true;

        // Handle proactive events
        if (this.options[id] && this.options[id].proactive && typeof this.proactive[id] !== "undefined") {
            callback.apply(this, this.proactive[id]);
            addToList = !once;
        }

        if (addToList) {
            id in this.list || (this.list[id] = []);
            this.list[id].push([callback, once]);
        }
        return this;
    }

    /// \brief Remove a specific callback previously inserted
    ///
    /// \param id The identifier of the event
    /// \param callback The function to be removed from the list
    remove(id: string, callback: (...args: any[]) => void): void {
        if (!(id in this.list)) {
            return;
        }
        const index = this.list[id].findIndex((item: [Function, boolean]) => item[0] === callback);
        if (index !== -1) {
            this.list[id].splice(index, 1);
        }
    }

    /// \brief Check if an event is active (only for proactive events)
    ///
    /// \param id The identifier of the events to be triggered
    ///
    /// \return true if the evetn has been triggered, false otherwise.
    is(id: string): boolean {
        return typeof this.proactive[id] !== "undefined";
    }

    /// \brief Trigger all events associated with a specific id
    ///
    /// \param id The identifier of the events to be triggered
    /// \param ... Arguments to be passed to the event callbacks
    trigger(id: string, ...args: any[]): Event {
        if (this.list[id]) {
            this.list[id].forEach((e: [Function, boolean]) => {
                e[0].apply(this, args);
            });
            this.list[id] = this.list[id].filter((e: [Function, boolean]) => {
                return e[1] !== true;
            });
        }

        // If this is a proactive event, save the call context for later
        if (this.options[id] && this.options[id].proactive) {
            this.proactive[id] = args || [];
        }
        return this;
    }

    /// \brief Clear a specific event or all events.
    ///
    /// Clearing events will remove the proactive trigger for example.
    /// It will also remove all previously attached events.
    clear(id?: string, onlyProactive?: boolean): Event {
        if (typeof id === "undefined") {
            this.proactive = {};
            this.list = {};
        } else {
            delete this.proactive[id];
            if (!onlyProactive) {
                delete this.list[id];
            }
        }
        return this;
    }

    /// Wait until the specific id is triggered
    async waitUntil(id: string): Promise<void> {
        return new Promise((resolve) => {
            this.on(
                id,
                () => {
                    resolve();
                },
                /*once*/ true,
            );
        });
    }
}