#!/usr/bin/env python3
import os
import pty
import select
import fcntl
import signal
import struct
import subprocess
import sys
import termios
import tty


resize_requested = False
child_process = None


def request_resize(signum: int, frame: object) -> None:
    global resize_requested
    resize_requested = True


def forward_termination(signum: int, frame: object) -> None:
    if child_process is None or child_process.poll() is not None:
        return
    try:
        os.killpg(child_process.pid, signum)
    except ProcessLookupError:
        return


def parse_env_size() -> tuple[int, int]:
    try:
        cols = int(os.environ.get("COLUMNS", "0") or "0")
        rows = int(os.environ.get("LINES", "0") or "0")
    except ValueError:
        return 0, 0
    if cols <= 0 or rows <= 0:
        return 0, 0
    return cols, rows


def ioctl_size(fd: int) -> tuple[int, int]:
    try:
        rows, cols, _, _ = struct.unpack("HHHH", fcntl.ioctl(fd, termios.TIOCGWINSZ, b"\0" * 8))
    except OSError:
        return 0, 0
    if cols <= 0 or rows <= 0:
        return 0, 0
    return cols, rows


def set_pty_size(fd: int) -> bool:
    cols, rows = parse_env_size()
    if cols == 0 or rows == 0:
        cols, rows = ioctl_size(sys.stdout.fileno())
    if cols == 0 or rows == 0:
        cols, rows = ioctl_size(sys.stdin.fileno())
    if cols == 0 or rows == 0:
        try:
            tty_fd = os.open("/dev/tty", os.O_RDONLY)
        except OSError:
            tty_fd = -1
        if tty_fd >= 0:
            try:
                cols, rows = ioctl_size(tty_fd)
            finally:
                os.close(tty_fd)
    if cols == 0 or rows == 0:
        return False
    fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack("HHHH", rows, cols, 0, 0))
    return True


def make_controlling_terminal(slave_fd: int) -> None:
    os.setsid()
    fcntl.ioctl(slave_fd, termios.TIOCSCTTY, 0)


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: pty_run.py <log-path> <command> [args...]", file=sys.stderr)
        return 2
    log_path = sys.argv[1]
    argv = sys.argv[2:]
    master_fd, slave_fd = pty.openpty()
    set_pty_size(slave_fd)
    global child_process
    proc = subprocess.Popen(
        argv,
        stdin=slave_fd,
        stdout=slave_fd,
        stderr=slave_fd,
        close_fds=True,
        preexec_fn=lambda: make_controlling_terminal(slave_fd),
    )
    child_process = proc
    os.close(slave_fd)
    signal.signal(signal.SIGWINCH, request_resize)
    signal.signal(signal.SIGINT, forward_termination)
    signal.signal(signal.SIGTERM, forward_termination)
    if hasattr(signal, "SIGHUP"):
        signal.signal(signal.SIGHUP, forward_termination)
    input_fd = sys.stdin.fileno()
    saved_input_mode = None
    if os.isatty(input_fd):
        saved_input_mode = termios.tcgetattr(input_fd)
        tty.setraw(input_fd)
    try:
        with open(log_path, "ab") as log:
            stdin_open = True
            while True:
                global resize_requested
                if resize_requested:
                    resize_requested = False
                    if set_pty_size(master_fd):
                        proc.send_signal(signal.SIGWINCH)
                read_fds = [master_fd]
                if stdin_open:
                    read_fds.append(sys.stdin.fileno())
                readable, _, _ = select.select(read_fds, [], [], 0.05)
                if stdin_open and sys.stdin.fileno() in readable:
                    try:
                        input_data = os.read(sys.stdin.fileno(), 8192)
                    except OSError:
                        input_data = b""
                    if input_data:
                        os.write(master_fd, input_data)
                    else:
                        stdin_open = False
                if master_fd in readable:
                    try:
                        data = os.read(master_fd, 8192)
                    except OSError:
                        data = b""
                    if data:
                        os.write(sys.stdout.fileno(), data)
                        log.write(data)
                        log.flush()
                if proc.poll() is not None:
                    while True:
                        try:
                            data = os.read(master_fd, 8192)
                        except OSError:
                            break
                        if not data:
                            break
                        os.write(sys.stdout.fileno(), data)
                        log.write(data)
                    break
    finally:
        if saved_input_mode is not None:
            termios.tcsetattr(input_fd, termios.TCSADRAIN, saved_input_mode)
        if proc.poll() is None:
            try:
                os.killpg(proc.pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
            try:
                proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                try:
                    os.killpg(proc.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                proc.wait(timeout=2)
        os.close(master_fd)
    return int(proc.returncode or 0)


if __name__ == "__main__":
    raise SystemExit(main())
