upload:
	pio run --target upload && sleep 1
	sudo picocom -b 115200 /dev/ttyACM0

upload:
	pio run
