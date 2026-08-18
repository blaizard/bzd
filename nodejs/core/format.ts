/// Dump the given object as a string for observability purposes.
///
/// Unlike JSON.stringify, it handles very large objects by printing only the first `maxSize` characters.
export function toDumpString(object: unknown, maxSize: number = 1000): string {
    const iterableToString = (
        iterable: Iterable<unknown>,
        sizeLeft: number,
        start: string,
        end: string
    ): string => {
        let result = start;
        let first = true;
        for (const subItem of iterable) {
            result += first ? "" : ", ";
            result += itemToString(subItem, sizeLeft - result.length);
            if (result.length >= sizeLeft) {
                return result;
            }
            first = false;
        }
        return result + end;
    };

    const dictionaryToString = (
        dictionary: Record<string, unknown>,
        sizeLeft: number,
        start: string,
        end: string
    ): string => {
        let result = start;
        let first = true;
        for (const [key, value] of Object.entries(dictionary)) {
            result += first ? "" : ", ";
            result += itemToString(key, sizeLeft - result.length) + ": ";
            result += itemToString(value, sizeLeft - result.length);
            if (result.length >= sizeLeft) {
                return result;
            }
            first = false;
        }
        return result + end;
    };

    const itemToString = (item: unknown, sizeLeft: number): string => {
        if (sizeLeft <= 0) {
            return "";
        }
        switch (typeof item) {
            case "object":
                if (item === null) {
                    return "null";
                }
                if (Array.isArray(item)) {
                    return iterableToString(item, sizeLeft, "[", "]");
                }
                if (item instanceof Set) {
                    return iterableToString(item, sizeLeft, "Set(", ")");
                }
                if (item.constructor === Object) {
                    return dictionaryToString(item as Record<string, unknown>, sizeLeft, "{", "}");
                }
                return "??";
            case "boolean":
                return item ? "true" : "false";
            case "number":
                return item.toString();
            case "bigint":
                return "<bigint>";
            case "string":
                return '"' + item.substring(0, sizeLeft) + '"';
            case "symbol":
                return "<symbol>";
            case "function":
                return "<function>";
            case "undefined":
            default:
                return "??";
        }
    };

    const string = itemToString(object, maxSize);
    if (string.length > maxSize) {
        return string.substring(0, maxSize) + "[...]";
    }
    return string;
}

interface ParsedFormat {
    index: number | string;
    metadata: string;
}

function _parseFormat(
    substitutionIndex: string,
    metadata: string,
    currentSubstitutionIndex: number
): ParsedFormat {
    const index = substitutionIndex ? parseInt(substitutionIndex, 10) : currentSubstitutionIndex;
    return {
        index: isNaN(index) ? substitutionIndex : index,
        metadata: metadata,
    };
}

/// Format a string using Python format syntax.
export default function formatString(str: string, ...args: unknown[]): string {
    const pattern = new RegExp("{([^}:]*)(?::([^}]*))?}", "g");
    let substitutionIndex = 0;

    const replacer = (_: string, name: string, metadata?: string): string => {
        // Handle the formatting operation for each match.
        const format = _parseFormat(name, metadata || "", substitutionIndex);
        ++substitutionIndex;

        // Resolve the value matching the placeholder.
        let value: unknown = null;
        if (typeof format.index === "string") {
            const a = args[0];
            if (!a || typeof a !== "object" || a.constructor !== Object) {
                throw new Error(
                    "Expected a dictionary as argument to match key '" + format.index + "' while formatting string: " + str
                );
            }
            const dict = a as Record<string, unknown>;
            if (!(format.index in dict)) {
                throw new Error("Missing key '" + format.index + "' while formatting string: " + str);
            }
            value = dict[format.index];
        } else {
            if (format.index >= args.length) {
                throw new Error("Too few arguments passed while formatting string: " + str);
            }
            value = args[format.index];
        }

        switch (format.metadata[0]) {
            case "?": {
                let maxSize = 1000;
                if ((format.metadata[1] ?? "").startsWith(".")) {
                    maxSize = parseInt(format.metadata.substring(2), 10);
                }
                return toDumpString(value, maxSize);
            }
            case undefined:
                return String(value);
            default:
                // Round the float with a specific precision.
                if (format.metadata.startsWith(".")) {
                    const precision = parseInt(format.metadata.substring(1), 10);
                    return Number(value).toFixed(precision);
                }
                // Pad the number with leading zeros.
                else if (format.metadata.startsWith("0")) {
                    const number = parseInt(format.metadata.substring(1), 10);
                    return String(value).padStart(number, "0");
                }
        }
        throw new Error("Unsupported formatting metadata: " + format.metadata);
    };

    return str.replace(pattern, replacer);
}