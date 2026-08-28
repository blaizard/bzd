/// Interface of the base64 encoding function used by the crypto module.
export interface Base64Encoder {
    (data: string): string;
}