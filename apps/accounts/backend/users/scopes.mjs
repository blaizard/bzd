import Scopes from "#bzd/nodejs/core/authentication/scopes.mjs";

/// Read, add and change anything related to users.
export const scopeAdminUsers = "admin-users";
/// Read, add and change anything related to applications.
export const scopeAdminApplications = "admin-applications";
/// Read basic information about the current user.
export const scopeSelfBasicRead = "self-basic:r";
/// Change basic information about the current user.
export const scopeSelfBasicWrite = "self-basic:w";
/// Read the current user roles.
export const scopeSelfRolesRead = "self-roles:r";
/// Update the current user roles.
export const scopeSelfRolesWrite = "self-roles:w";
/// Read the current user tokens.
export const scopeSelfTokensRead = "self-tokens:r";
/// Change the current user tokens.
export const scopeSelfTokensWrite = "self-tokens:w";
/// Read the current user subscriptions.
export const scopeSelfSubscriptionsRead = "self-subscriptions:r";
/// Change the current user subscriptions.
export const scopeSelfSubscriptionsWrite = "self-subscriptions:w";

/// All the scopes available
export const allScopes = new Scopes([
	scopeAdminUsers,
	scopeAdminApplications,
	scopeSelfBasicRead,
	scopeSelfBasicWrite,
	scopeSelfRolesRead,
	scopeSelfRolesWrite,
	scopeSelfTokensRead,
	scopeSelfTokensWrite,
	scopeSelfSubscriptionsRead,
	scopeSelfSubscriptionsWrite,
]);
