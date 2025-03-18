device = serialport("COM8",9600);
datatype = "string";

for i = 1:100
    data = read(device,25,datatype)
end