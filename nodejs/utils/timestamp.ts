export function timestampMs(): number {
    return Date.now();
}

export function timestampUs(): number {
    return Math.round(performance.timeOrigin * 1000 + performance.now() * 1000);
}

export function timestampS(): number {
    return Date.now() / 1000;
}