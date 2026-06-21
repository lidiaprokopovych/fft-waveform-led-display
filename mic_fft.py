"""
Mic Spectrum Analyzer (Python)
--------------------------------
Visualizes a live frequency spectrum from your microphone, computed via
a Fourier transform (FFT) of the most recent audio buffer.

Install dependencies:
    pip install sounddevice matplotlib numpy

Linux only — you may also need the PortAudio runtime library:
    sudo apt-get install libportaudio2

Run:
    python mic_oscilloscope.py

A window will open showing your microphone's frequency spectrum updating
in real time. Close the window (or press Ctrl+C in the terminal) to stop.
"""

import sys
import numpy as np
import sounddevice as sd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ---- Configuration ----
SAMPLE_RATE = 44100
FFT_SIZE = 2048              # samples per FFT window (~46 ms at 44.1kHz)
CALLBACK_BLOCK = 1024        # frames sounddevice hands us per callback
DEBUG = True                 # prints peak level to the terminal periodically
MIN_FREQ = 20                # Hz, lower bound of plotted spectrum
MAX_FREQ = SAMPLE_RATE / 2   # Nyquist frequency, upper bound

# Rolling buffer that always holds the most recent FFT_SIZE samples
buffer = np.zeros(FFT_SIZE, dtype=np.float32)
window = np.hanning(FFT_SIZE)               # reduces spectral leakage
freqs = np.fft.rfftfreq(FFT_SIZE, d=1.0 / SAMPLE_RATE)
_callback_count = 0


def audio_callback(indata, frames, time_info, status):
    """Called by sounddevice on a background thread for every new chunk
    of audio. Shifts the new samples into the rolling buffer."""
    global buffer, _callback_count
    if status:
        print(status, file=sys.stderr)

    data = indata[:, 0]  # mono channel
    n = len(data)

    if n >= FFT_SIZE:
        buffer[:] = data[-FFT_SIZE:]
    else:
        buffer[:-n] = buffer[n:]
        buffer[-n:] = data

    if DEBUG:
        _callback_count += 1
        if _callback_count % 20 == 0:  # roughly twice a second
            peak = np.max(np.abs(data))
            print(f"peak level: {peak:.4f}", flush=True)


def compute_spectrum():
    """Returns the magnitude spectrum (in dB) of the current buffer via FFT."""
    windowed = buffer * window
    spectrum = np.fft.rfft(windowed)
    magnitude = np.abs(spectrum) / FFT_SIZE
    magnitude_db = 20 * np.log10(magnitude + 1e-6)
    return magnitude_db


def main():
    plt.style.use('dark_background')
    fig, ax = plt.subplots(figsize=(9, 4.5))
    fig.patch.set_facecolor('#050a07')
    ax.set_facecolor('#081710')

    initial_db = compute_spectrum()
    line, = ax.plot(freqs, initial_db, color='#4dff9e', linewidth=1.4)

    ax.set_xscale('log')
    ax.set_xlim(MIN_FREQ, MAX_FREQ)
    ax.set_ylim(-100, -40)
    ax.set_xlabel('Frequency (Hz)', color='#7fdb9c')
    ax.set_ylabel('Magnitude (dB)', color='#7fdb9c')
    ax.set_title('Mic Spectrum Analyzer — Live FFT', color='#d6ffe9')
    ax.tick_params(colors='#5f9c7c')
    ax.grid(True, which='both', color='#163322', linewidth=0.6)
    for spine in ax.spines.values():
        spine.set_color('#1d4a31')

    def update(_frame):
        line.set_ydata(compute_spectrum())
        return (line,)

    default_input = sd.query_devices(kind='input')
    print(f"Using input device: {default_input['name']}", flush=True)

    try:
        stream = sd.InputStream(
            channels=1,
            samplerate=SAMPLE_RATE,
            blocksize=CALLBACK_BLOCK,
            callback=audio_callback,
        )
    except Exception as exc:
        print(f"Could not open microphone: {exc}", file=sys.stderr)
        print("Run `python -m sounddevice` to list available input devices.")
        sys.exit(1)

    with stream:
        ani = animation.FuncAnimation(
            fig, update, interval=30, blit=True, cache_frame_data=False
        )
        plt.show()


if __name__ == '__main__':
    main()