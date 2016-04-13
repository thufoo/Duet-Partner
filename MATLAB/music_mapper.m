L= 1024*4;
k = 0:L-1;
toast = exp(-i*pi/L.*k);

base = log2(L);

%create twiddle factors
Wnk = zeros(1,L);
for n = 0:L-1
    Wnk(bin2dec (fliplr(dec2bin(n,base)))+1) = toast(n+1);
end
    
 SampleRate = 44100;%input('Please choose a sample rate : ');
 fft_length = L;%input('Please choose a fft length: ');
 lin_or_db = 0;%input('Please choose a linear or dB scale (0 or 1): ');
 FrameSize = fft_length;

 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 alpha = .3; %amount of averaging
 prevOutput = zeros(fft_length,1);
 f = (SampleRate/2)*linspace(0,1,fft_length/2+1);
 first = find(f > 240,1);%C4
 last = find(f > 1046,1);%C6
 bin_size = SampleRate/fft_length;
 time_step = 1/bin_size/2;%divide by two for overlap-save of 50%
 prev_note = ' ';
 cur_note = ' ';
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 data = audioread('chromatic.wav');%wavread('mix10.wav');%wavread for older MATLAB versions
 max_level = max(abs(data(:,1)));
 data = data*1/max_level;%normalize data
% This records the first set of data
disp('Starting processing');
tic;
for n=1:2*(length(data)/fft_length)-1

    input_data = data((n-1)*fft_length/2+1:(n-1)*fft_length/2+1+fft_length-1,1);

    %tic;
    xF = fft(input_data(:,1),fft_length);
    %xF = inPlaceFFT(input_data(:,1),Wnk,fft_length)';
    %toc;
    output = real(xF).^2+imag(xF).^2;

    %output = (1-alpha)*output+alpha*prevOutput;
    prevOutput = output;
    
    %Plot data
    figure(1);
    semilogy(f, output(1:fft_length/2+1));
    xlim([f(first/2),f(last*2)]);
    xlabel('Frequency in Hz');
    ylabel('Amplitude');
    ylim([1E-4, 5E5]);
    drawnow;
    
    %for false positives, check at 1.5*freq--if there's something there,
    %you're looking at a harmonic!

    %Process data
    cur_max = max(output(first:last));
    if(cur_max > 2E3)
        max_bin = find(output(first:last) == cur_max,1);
        freq_found = (max_bin-1+first-1)*bin_size;  
        if(output(floor((max_bin+first)*0.5)) > cur_max*0.2)
            freq_found = freq_found/2;
        end
        cur_note = get_note(freq_found);
        if(cur_note == prev_note)
            %do nothing
        else
            time = time_step*(n-1);
            if(prev_note ~= ' ')
                disp([prev_note, '      stops ', num2str(time)]);
            end
            disp([cur_note, '     starts ', num2str(time)]);
            prev_note = cur_note;
        end
    else
        if(cur_note ~= ' ')
            time = time_step*(n-1);
            disp([cur_note, '     stops ', num2str(time)]);
            prev_note = ' ';
            cur_note = ' ';
        end
    end

end
toc;