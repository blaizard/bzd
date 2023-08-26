export default function uncaughtExceptionHandler(ExceptionFactory) {
    window.addEventListener("error", (e) => {
        ExceptionFactory.fromError(e).print();
        return false;
    });
}
