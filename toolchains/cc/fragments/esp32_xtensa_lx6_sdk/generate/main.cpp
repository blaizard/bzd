extern "C" void app_main();
extern int main();

void app_main()
{
	main();
	// No need for a while (1) loop here, it will be caught be the under layer.
	// Also a while (1) here will trigger the watchdog.
}
