%% Simulation World
% Engineer: Tim Brothers
% Overview
%    This is a simulation for the Robotic Orchestra
% Design Name:   The Conductor
% File Name:     simulation_world.m
%
% Inputs: 
%		Tempo: controls the tempo of the song. This is a number between 0 and 1
%		Octave: The ocive of the song (normally 4)
%		fs_kHz: sampling rate in kHz.
%       tempo_resolution: this is a value between 0 and 1. 1 being full 
%           resolution. Be careful setting this too high. 1 will lock up
%           your computer.
%       time_offset: This is the delay to the start of the song. This is an
%           index to the time scale, so the actual delay amount is
%           determined by multiple factors
%
% History:       4 January 2020 File created
%
%-----------------------------------------------------

%% Clear the Variables
clc
clear
close all

%% Set up the world
fs_kHz = 5;

% Set the tempo and the octave for the conductor
Octave = 8;
tempo_s = 0.6; %This is the time in seconds for each note

%% Parameters for code simulation
tempo_resolution = .05; % This is a value between 0 and 1. 1 being full resolution
time_offset = 5000;   % This is the delay to the start of the song.

%% Create the song given the tempo
[song_freq_Hz, song_duration_s] = conductor_simulation(tempo_s,Octave);

% form up the cumulative durations
cumulative_duration_s = zeros(1, length(song_duration_s));
cumulative_duration_s(1) =  song_duration_s(1);
for i=2:length(song_duration_s)
	cumulative_duration_s(i) = song_duration_s(i) + cumulative_duration_s(i-1);
end

total_duration_s = max(cumulative_duration_s);
fs_Hz = fs_kHz*1000; %Convert kHz to Hz
time_s = 0:(1/fs_Hz):total_duration_s;	%create a time vector

% There is some rounding error happening sometimes, so we are going to make
% the end value exact
time_s(length(time_s)) = total_duration_s;


%% Create the digital signal
min_time_index = 1; % start the min time index at the start
% loop through all the freqs and generate a sin wave for each freq
for i=1:length(song_freq_Hz)
	max_time_index = find(time_s >= cumulative_duration_s(i),1); %find the times that correspond to this duration of time
	digital_note = sin(2*pi*song_freq_Hz(i)*time_s(min_time_index:max_time_index)); %create the actual note
	min_time_index = max_time_index + 1; %shift to the next time region.
	if(i == 1)
        digital = digital_note;
    else
        digital = [digital, digital_note];
    end
    
end

%% Create a signal to test the system
% Put two copies of the song together
signal = [digital, digital];
signal = circshift(signal, time_offset);  % circularly shifts array


%% Plot the Time Domain
plot(time_s,digital)  % unshifted signal
xlabel("time (s)")
ylabel("Amplitude")
title("Row Row Row Your Boat")

%% Plot the frequency
figure()
freq_digital = fft(digital);
freq_digital = freq_digital(1:floor(length(digital)/2));
freq_axis_kHz = linspace(0, fs_kHz/2, length(freq_digital));
plot(freq_axis_kHz, abs(freq_digital))
xlabel("Frequency (kHz)")
ylabel("Amplitude")
title("Spectrum of Row Row Row Your Boat")

%% Do the envelope detection
% don't need entire length of signal
signal = signal(1:round(length(signal)/3));
time_signal = 0:1/fs_Hz:length(signal)/fs_Hz;
downsample_factor = 22;
signal_sq = signal .* signal;
sigenv = sqrt(lowpass(downsample(signal_sq, downsample_factor), 10, fs_Hz));
time_samp = downsample(time_signal, downsample_factor);
real_sigenv = real(sigenv);
plot(time_samp, real_sigenv);
% https://www.mathworks.com/help/dsp/ug/envelope-detection.html

%% Find the Tempo
threshold = 0.5;
digital_env = real_sigenv > threshold;  % digitize
subplot(2,1,1);
plot(time_samp, digital_env);
xlabel('Time (sec)');
ylabel('Envelope');
ylim([-.1,1.1]);
title('Offset Signal');
subplot(2,1,2);
plot(time_s, digital);
xlabel('Time (sec)');
ylabel('Amplitude');
ylim([-1.1,1.1]);
title('Non-Delayed Signal')

% find rests
rest_indices = find(digital_env == 0);
toggle_indices = [];
for i = 2:length(rest_indices)-1
   if rest_indices(i-1) ~= rest_indices(i)-1 || rest_indices(i+1) ~= rest_indices(i)+1
       toggle_indices = [toggle_indices rest_indices(i)];
   end
end
toggle_indices;

% find rests and average time length
total_rests = 0;
rest_time = 0;
for i = 1:length(toggle_indices)-2
   if digital_env(toggle_indices(i)+1) == 0
       total_rests = total_rests + 1;
       rest_time = rest_time + time_samp(toggle_indices(i+1)) - time_samp(toggle_indices(i));
   end
end
% 1 beat time
tempo_calc = rest_time/total_rests;

%% Sync the time
note_lengths = [];
for i = 2:length(toggle_indices)-1
    note_lengths = [note_lengths, time_samp(toggle_indices(i+1)) - time_samp(toggle_indices(i))];
end

note_lengths = round(note_lengths ./ tempo_calc);
beats = [2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1];
% find where note_lengths exists in beats
start_index = strfind(beats, note_lengths);
time = 0;
for i = 1:start_index-1
    time = time + tempo_calc*beats(i);
end
calc_time_offset = time_samp(toggle_indices(start_index)) - time;


actual_time_offset_s = time_s(time_offset); %print out the actual start time to compare.

fprintf('Calculated Time Offset: %f\t', calc_time_offset);
fprintf('Actual Time Offset: %f\n', actual_time_offset_s);
fprintf('Time Offset Delta: %f\n', calc_time_offset - actual_time_offset_s);
fprintf('Calculated Tempo: %f\t', tempo_calc);
fprintf('Actual Tempo: %f\n', tempo_s);
fprintf('Tempo Delta: %f\n', tempo_calc - tempo_s);
