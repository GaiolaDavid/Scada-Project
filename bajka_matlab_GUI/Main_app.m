classdef Main_app < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure           matlab.ui.Figure
        
        MainPanel          matlab.ui.container.Panel
        HistoryPanel       matlab.ui.container.Panel
        %-----------------------MAIN_APP--------------------%
        WarningLabel       matlab.ui.control.Label      
        LiveDataLabel      matlab.ui.control.Label
        SpeedGauge         matlab.ui.control.Gauge
        HumidityLabel      matlab.ui.control.Label
        HumidityImage      matlab.ui.control.Image
        SpeedrpmLabel      matlab.ui.control.Label
        SpeedNumField      matlab.ui.control.NumericEditField
        TemperatureCLabel  matlab.ui.control.Label
        TemperatureImage   matlab.ui.control.Image
        DistanceImage      matlab.ui.control.Image
        FuelImage          matlab.ui.control.Image
        FuellevelLabel     matlab.ui.control.Label
        DistancecmLabel    matlab.ui.control.Label
        FuelNumField       matlab.ui.control.NumericEditField
        TempGauge          matlab.ui.control.LinearGauge
        TempNumField       matlab.ui.control.NumericEditField
        DistanceNumField   matlab.ui.control.NumericEditField
        FuelGauge          matlab.ui.control.NinetyDegreeGauge
        HumidityGauge      matlab.ui.control.LinearGauge
        HumidityNumField   matlab.ui.control.NumericEditField
        CarImage           matlab.ui.control.Image
        PastDataButton     matlab.ui.control.Button
        %---------------------HISTORY_APP--------------------%
        BackButton    matlab.ui.control.Button
        LoadButton    matlab.ui.control.Button
        TypeDropDown  matlab.ui.control.DropDown
        UIAxes        matlab.ui.control.UIAxes

        serialObj          
        saveTimer          timer
        dataTable          table
    end
    
    properties (Access = private)
        isCollectingData logical = true
    end
    

    
    methods (Access = private)
        
        function readSerialData(app, ~, ~)
            % Read and process serial data
            rawData = readline(app.serialObj);
            if startsWith(rawData, ":")
                parsedData = sscanf(rawData, ':%c%04d%04d%04d%02d%02d%02d%02d%02d');
                if numel(parsedData) == 9
                    type = string(char(parsedData(1)));
                    a = parsedData(2);
                    b = parsedData(3);
                    year = parsedData(4);
                    month = parsedData(5);
                    day = parsedData(6);
                    hour = parsedData(7);
                    minute = parsedData(8);
                    second = parsedData(9);
                    
                    timestamp = datetime(year, month, day, hour, minute, second);
                    floatValue = a + (b / 10000); % Convert to decimal

                    switch type
                        case "t"
                            app.TempGauge.Value = floatValue;
                            app.TempNumField.Value = floatValue;

                        case "f"
                            app.FuelGauge.Value = floatValue;
                            app.FuelNumField.Value = floatValue;

                        case "h"
                            app.HumidityGauge.Value = floatValue;
                            app.HumidityNumField.Value = floatValue;
                        
                        case "r"
                            app.SpeedGauge.Value = floatValue;
                            app.SpeedNumField.Value = floatValue;
                        case "u"
                            app.DistanceNumField.Value = floatValue;
                        otherwise
                            disp("Unknown data type received");
                    end

                    % Store data
                    newEntry = table(timestamp, type, floatValue, 'VariableNames', ["Timestamp", "Type", "Value"]);
                    app.dataTable = [app.dataTable; newEntry];

                    % Update GUI
                    
                end
            end
        end 

        function saveDataPeriodically(app)
            if app.isCollectingData && ~isempty(app.dataTable)
                disp("Saving data...");
        
                % Load existing data if the file exists
                if isfile("saved_data.mat")
                    loadedData = load("saved_data.mat", "savedTable");
                    if isfield(loadedData, "savedTable")
                        existingTable = loadedData.savedTable;
                    else
                        existingTable = table([], [], [], 'VariableNames', ["Timestamp", "Type", "Value"]);
                    end
                else
                    existingTable = table([], [], [], 'VariableNames', ["Timestamp", "Type", "Value"]);
                end
        
                % Append new data
                savedTable = [existingTable; app.dataTable];
        
                % Save updated table back to the .mat file
                save("saved_data.mat", "savedTable");
        
                % Clear the contents of app.dataTable after saving
                app.dataTable = table([], [], [], 'VariableNames', ["Timestamp", "Type", "Value"]);
            else
                disp("No data to save.");
            end
        end

        function closeRequest(app, ~)
            disp("Closing application...");
            
            app.isCollectingData = false; % Stop saving loop
            if ~isempty(app.serialObj) && isobject(app.serialObj) && isvalid(app.serialObj)
                configureCallback(app.serialObj, "off"); % Stop serial callback
            end
            stop(app.saveTimer);  % Stop the timer
            delete(app.saveTimer); % Delete the timer
            delete(app);
        end

        % Code that executes after component creation
        function startupFcn(app)

            app.UIFigure.CloseRequestFcn = @(src, event) closeRequest(app, src);
        
            % Setup serial port
            try
                serial_devices = serialportlist("available");
                app.serialObj = serialport(serial_devices(1), 9600);
                configureTerminator(app.serialObj, "LF");
                configureCallback(app.serialObj, "terminator", @(src, event) readSerialData(app));

                app.WarningLabel.Visible = "off";
            catch
                app.serialObj = [];
                app.WarningLabel.Text = 'Failed to connect!';
                app.WarningLabel.Visible = 'on'; 
            end
            % Initialize table
            app.dataTable = table([], [], [], 'VariableNames', ["Timestamp", "Type", "Value"]);

            savedTable = table([], [], [], 'VariableNames', ["Timestamp", "Type", "Value"]);
            save('saved_data.mat', 'savedTable');

            % Create a timer object to call saveDataPeriodically every 10 seconds
            app.saveTimer = timer('ExecutionMode', 'fixedRate','Period', 10, 'StartDelay', 10,'TimerFcn', @(src, event) saveDataPeriodically(app));

            % Start the timer
            start(app.saveTimer);
            app.MainPanel.Visible = 'on';
            app.HistoryPanel.Visible = 'off';
        end

        function switchScreen(app, screenName)
            disp("Switching");
            if screenName == "history"
                app.MainPanel.Visible = "on";
                app.HistoryPanel.Visible = "off";
            else
                app.MainPanel.Visible = "off";
                app.HistoryPanel.Visible = "on";
            end
        end

        function loadData(app)
            if isfile('saved_data.mat')
                disp("loading");
                s = load('saved_data.mat');
                if isfield(s, 'savedTable')
                    loadedData = s.savedTable;
                    app.updatePlot(loadedData);
                else
                    uialert(app.UIFigure, 'savedTable not found in MAT file', 'Error');
                end
            else
                uialert(app.UIFigure, 'saved_data.mat not found', 'Error');
            end
        end
        
        function updatePlot(app, loadedData)
            if isempty(loadedData)
                return;
            end
            selectedType = app.TypeDropDown.Value;
            filtered = loadedData(loadedData.Type == selectedType, :);

            index = find(strcmp(app.TypeDropDown.ItemsData, selectedType)); % Find index of the selected value
            selectedText = app.TypeDropDown.Items{index}; % Get the corresponding full text
            
            if isempty(filtered)
                cla(app.UIAxes); % Clear axes if no data
                title(app.UIAxes, 'No data for selected type');
            else
                plot(app.UIAxes, filtered.Timestamp, filtered.Value, '-o');
                title(app.UIAxes, sprintf('Data for %s', selectedText));
                xlabel(app.UIAxes, 'Timestamp');
                ylabel(app.UIAxes, 'Value');
                grid(app.UIAxes, 'on');
            end
        end

    end

    % Component initialization
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Get the file path for locating images
            pathToMLAPP = fileparts(mfilename('fullpath'));

            % Create UIFigure and hide until all components are created
            app.UIFigure = uifigure('Visible', 'off');
            app.UIFigure.AutoResizeChildren = 'off';
            app.UIFigure.Color = [0.8 0.8 0.8];
            app.UIFigure.Position = [100 100 640 480];
            app.UIFigure.Name = 'SCADA';
            app.UIFigure.Resize = 'off';

            % Create Panel Container (Stacked)
            app.MainPanel = uipanel(app.UIFigure, 'Position', [0, 0, 640, 480], 'Visible', 'on');
            app.HistoryPanel = uipanel(app.UIFigure, 'Position', [0, 0, 640, 480], 'Visible', 'off');
            %% -------------------- MAIN SCREEN COMPONENTS -------------------- %%
            % Create Image
            app.CarImage = uiimage(app.MainPanel);
            app.CarImage.Position = [153 190 335 241];
            app.CarImage.ImageSource = fullfile(pathToMLAPP, 'autocska.png');

            % Create HumidityNumField
            app.HumidityNumField = uieditfield(app.MainPanel, 'numeric');
            app.HumidityNumField.Position = [531 42 44 22];

            % Create HumidityGauge
            app.HumidityGauge = uigauge(app.MainPanel, 'linear');
            app.HumidityGauge.Orientation = 'vertical';
            app.HumidityGauge.Position = [532 72 41 119];

            % Create FuelGauge
            app.FuelGauge = uigauge(app.MainPanel, 'ninetydegree');
            app.FuelGauge.Position = [508 320 90 90];

            % Create DistanceNumField
            app.DistanceNumField = uieditfield(app.MainPanel, 'numeric');
            app.DistanceNumField.Editable = 'off';
            app.DistanceNumField.Position = [54 287 70 19];

            % Create TempNumField
            app.TempNumField = uieditfield(app.MainPanel, 'numeric');
            app.TempNumField.Position = [82 42 44 22];

            % Create TempGauge
            app.TempGauge = uigauge(app.MainPanel, 'linear');
            app.TempGauge.Limits = [-20 80];
            app.TempGauge.MajorTicks = [-20 0 20 40 60 80];
            app.TempGauge.Orientation = 'vertical';
            app.TempGauge.Position = [83 72 36 120];

            % Create FuelNumField
            app.FuelNumField = uieditfield(app.MainPanel, 'numeric');
            app.FuelNumField.Position = [554 284 44 22];

            % Create DistancecmLabel
            app.DistancecmLabel = uilabel(app.MainPanel);
            app.DistancecmLabel.FontSize = 18;
            app.DistancecmLabel.Position = [29 320 116 23];
            app.DistancecmLabel.Text = 'Distance (cm)';

            % Create FuellevelLabel
            app.FuellevelLabel = uilabel(app.MainPanel);
            app.FuellevelLabel.FontSize = 18;
            app.FuellevelLabel.Position = [508 422 115 23];
            app.FuellevelLabel.Text = 'Fuel level (%)';

            % Create Image2 
            app.FuelImage = uiimage(app.MainPanel);
            app.FuelImage.Position = [508 278 38 35];
            app.FuelImage.ImageSource = fullfile(pathToMLAPP, 'fuel.png');

            % Create Image3
            app.DistanceImage = uiimage(app.MainPanel);
            app.DistanceImage.Position = [123 257 89 79];
            app.DistanceImage.ImageSource = fullfile(pathToMLAPP, 'distance.png');

            % Create Image4
            app.TemperatureImage = uiimage(app.MainPanel);
            app.TemperatureImage.Position = [1 83 100 100];
            app.TemperatureImage.ImageSource = fullfile(pathToMLAPP, 'temperature.png');

            % Create TemperatureCLabel
            app.TemperatureCLabel = uilabel(app.MainPanel);
            app.TemperatureCLabel.FontSize = 18;
            app.TemperatureCLabel.Position = [29 203 143 23];
            app.TemperatureCLabel.Text = 'Temperature (°C)';

            % Create SpeedNumField
            app.SpeedNumField = uieditfield(app.MainPanel, 'numeric');
            app.SpeedNumField.Position = [299 42 44 22];

            % Create SpeedrpmLabel
            app.SpeedrpmLabel = uilabel(app.MainPanel);
            app.SpeedrpmLabel.FontSize = 18;
            app.SpeedrpmLabel.Position = [268 203 105 23];
            app.SpeedrpmLabel.Text = 'Speed (rpm)';

            % Create Image5
            app.HumidityImage = uiimage(app.MainPanel);
            app.HumidityImage.Position = [457 96 75 70];
            app.HumidityImage.ImageSource = fullfile(pathToMLAPP, 'humidity.png');

            % Create HumidityLabel
            app.HumidityLabel = uilabel(app.MainPanel);
            app.HumidityLabel.FontSize = 18;
            app.HumidityLabel.Position = [489 203 108 23];
            app.HumidityLabel.Text = 'Humidity (%)';

            % Create SpeedGauge
            app.SpeedGauge = uigauge(app.MainPanel, 'circular');
            app.SpeedGauge.Limits = [0 250];
            app.SpeedGauge.Position = [261 73 120 120];

            % Create LiveDataLabel
            app.LiveDataLabel = uilabel(app.MainPanel);
            app.LiveDataLabel.FontName = 'Calibri';
            app.LiveDataLabel.FontSize = 48;
            app.LiveDataLabel.FontWeight = 'bold';
            app.LiveDataLabel.Position = [227 401 187 64];
            app.LiveDataLabel.Text = 'Live Data';

            % Create PastDataButton
            app.PastDataButton = uibutton(app.MainPanel, 'push');
            app.PastDataButton.FontSize = 24;
            app.PastDataButton.Position = [37 414 127 38];
            app.PastDataButton.Text = 'Past Data';
            app.PastDataButton.ButtonPushedFcn = @(src, event) app.switchScreen("main");

            % Create WarningLabel
            app.WarningLabel = uilabel(app.MainPanel);
            app.WarningLabel.FontSize = 18;
            app.WarningLabel.FontColor = [1 0 0];
            app.WarningLabel.Position = [250 368 179 34];
            app.WarningLabel.Text = '';

            %% -------------------- HISTORY SCREEN COMPONENTS -------------------- %%

            % Create UIAxes
            app.UIAxes = uiaxes(app.HistoryPanel);
            title(app.UIAxes, 'Title')
            xlabel(app.UIAxes, 'X')
            ylabel(app.UIAxes, 'Y')
            zlabel(app.UIAxes, 'Z')
            app.UIAxes.Position = [71 120 500 330];

            % Create TypeDropDown
            app.TypeDropDown = uidropdown(app.HistoryPanel);
            app.TypeDropDown.Position = [278 80 109 30];
            app.TypeDropDown.Items = {'Fuel level', 'Temperature', 'Humidity', 'Speed', 'Distance'};
            app.TypeDropDown.ItemsData = {'f', 't', 'h', 'r', 'u'};
            % Create LoadButton
            app.LoadButton = uibutton(app.HistoryPanel, 'push');
            app.LoadButton.Position = [446 80 107 30];
            app.LoadButton.ButtonPushedFcn = @(src, event) app.loadData();
            app.LoadButton.Text = "Load Data";

            % Create BackButton
            app.BackButton = uibutton(app.HistoryPanel, 'push');
            app.BackButton.Position = [112 80 107 30];
            app.BackButton.ButtonPushedFcn = @(src, event) app.switchScreen("history");
            app.BackButton.Text = "Back";

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = Main_app

            % Create UIFigure and components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            % Execute the startup function
            runStartupFcn(app, @startupFcn)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end