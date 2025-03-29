device = serialport("COM7", 9600); % Open serial port
configureTerminator(device, "LF"); % Set Line Feed (\n) as the terminator
flush(device); % Clear the buffer before starting

for i = 1:100
    data = readline(device); % Read full line until \n
    disp(data); % Display the full string
end