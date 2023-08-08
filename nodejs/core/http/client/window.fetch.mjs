import ExceptionFactory from "../../exception.mjs";

const Exception = ExceptionFactory("http", "client", "window.fetch");

export default async function request(url, options) {
  // Add support for FormData
  if (options.data instanceof FormData) {
	options.headers["Content-Type"] = "application/x-www-form-urlencoded";
	options.data = JSON.stringify(options.data);
  }

  const promiseFetch = window.fetch(url, {
	method : options.method,
	body : options.data,
	headers : options.headers,
  });

  const promiseTimeout = new Promise((resolve) => {
	setTimeout(resolve, options.timeoutMs, {
	  timeout : true,
	});
  });

  const response = await Promise.race([ promiseFetch, promiseTimeout ]);
  if (response.timeout) {
	throw new Exception("Request timeout (" + options.timeoutMs + " ms)");
  }

  return {
	data : options.expect == "stream" ? response.body : await response.text(),
	headers : [...response.headers ].reduce(
		(obj, pair) => {
		  obj[pair[0]] = pair[1];
		  return obj;
		},
		{}),
	code : response.status,
  };
}
