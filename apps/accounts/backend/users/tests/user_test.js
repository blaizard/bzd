import ExceptionFactory from "#bzd/nodejs/core/exception.js";
import KeyValueStoreMemory from "#bzd/nodejs/db/key_value_store/memory.js";
import User from "#bzd/apps/accounts/backend/users/user.js";
import Users from "#bzd/apps/accounts/backend/users/users.js";

const Exception = ExceptionFactory("test", "user");

describe("User", () => {
	describe("password reset token", () => {
		const makeUser = () => User.create("uid", "user@dummy.com");

		it("accepts a valid token", () => {
			const user = makeUser();
			const token = "a-valid-token";
			user.setPasswordResetToken(token);
			Exception.assert(user.checkPasswordResetToken(token), "A valid token must be accepted.");
		});

		it("rejects a wrong token", () => {
			const user = makeUser();
			user.setPasswordResetToken("a-valid-token");
			Exception.assert(!user.checkPasswordResetToken("a-wrong-token"), "A wrong token must be rejected.");
		});

		it("rejects a token without any reset entry", () => {
			const user = makeUser();
			Exception.assert(
				!user.checkPasswordResetToken("a-valid-token"),
				"A token without a reset entry must be rejected.",
			);
		});

		it("rejects an expired token", () => {
			const user = makeUser();
			user.setPasswordResetToken("a-valid-token", /*durationMs*/ -1000);
			Exception.assert(!user.checkPasswordResetToken("a-valid-token"), "An expired token must be rejected.");
		});

		it("clears the token after use", () => {
			const user = makeUser();
			const token = "a-valid-token";
			user.setPasswordResetToken(token);
			user.clearPasswordResetToken();
			Exception.assert(!user.checkPasswordResetToken(token), "A cleared token must be rejected.");
		});

		it("replaces the token on regeneration", () => {
			const user = makeUser();
			const token1 = "token-1";
			const token2 = "token-2";
			user.setPasswordResetToken(token1);
			user.setPasswordResetToken(token2);
			Exception.assert(!user.checkPasswordResetToken(token1), "A rotated token must be rejected.");
			Exception.assert(user.checkPasswordResetToken(token2), "The new token must be accepted.");
		});

		it("does not store the raw token", () => {
			const user = makeUser();
			user.setPasswordResetToken("a-valid-token");
			Exception.assert(user.data().password_reset.hash !== "a-valid-token", "The raw token must not be stored.");
		});

		it("does not expose the password reset data publicly", () => {
			const user = makeUser();
			user.setPasswordResetToken("a-valid-token");
			Exception.assert(!("password_reset" in user.dataPublic()), "The reset data must not be exposed.");
			Exception.assert(!("password" in user.dataPublic()), "The password hash must not be exposed.");
		});
	});

	describe("_preprocessAndMergePublic", () => {
		let users;
		before(async () => {
			const keyValueStore = await KeyValueStoreMemory.make("users-test");
			users = new Users(keyValueStore);
		});

		const makeUserWithPassword = async (email) => {
			const user = await users.create(email);
			return await users.update(user.getUid(), async (u) => {
				await u.setPassword("1234");
				return u;
			});
		};

		it("accepts the real password as oldpassword", async () => {
			const user = await makeUserWithPassword("dummy-1@dummy.com");
			await users.update(user.getUid(), async (u) => {
				return await users._preprocessAndMergePublic({ password: "5678", oldpassword: "1234" }, u);
			});
		});

		it("rejects the stored bcrypt hash as oldpassword", async () => {
			const user = await makeUserWithPassword("dummy-2@dummy.com");
			const hash = user.getPassword();
			Exception.assert(hash, "The user must have a password hash.");
			await Exception.assertThrows(async () => {
				await users.update(user.getUid(), async (u) => {
					return await users._preprocessAndMergePublic({ password: "5678", oldpassword: hash }, u);
				});
			}, "Old password is different");
		});
	});
});
