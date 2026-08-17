export function getQueryAsDict(): Record<string, string> {
    const params = new URLSearchParams(window.location.search);
    return Object.fromEntries(params.entries());
}
