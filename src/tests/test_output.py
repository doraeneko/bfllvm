########################################
# bfllvm
# Test output of demo bf programs
# using pytest.
########################################
import os
import subprocess
import pytest
import shutil
import pathlib


@pytest.mark.parametrize(
    "test_file, intermediate, expected_return_code, expected_output",
    [
        ("hello_world.bf", "intermediate.bf", 0, "Hello, World!"),
        ("invalid.bf", "intermediate.bf", 1, None),
        ("output_h.bf", "intermediate.bf", 0, "H\n"),
    ],
)
def test_executable_output_in_temp_dir(
    tmp_path, test_file, intermediate, expected_return_code, expected_output
):
    """Test that the executable's output matches the spec when run in a temporary directory."""
    # Path to the built executable
    executable_src_path = (
        pathlib.Path(os.path.dirname(__file__)) / "../../build/bfllvm"
    )  # Modify this path as needed
    test_file = pathlib.Path(os.path.dirname(__file__)) / test_file
    # Create a temporary directory where the executable will run
    temp_dir = tmp_path

    file_content = ""
    with open(test_file, "r") as file:
        file_content = file.read()

    # Copy the executable into the temporary directory
    executable_temp_path = temp_dir / "bfllvm"
    try:
        shutil.copy(executable_src_path, executable_temp_path)
    except FileNotFoundError:
        print("Could not find bfllvm executable in build dir. Have you built it?")
        assert False

    result = subprocess.run(
        [executable_temp_path, "-o", intermediate],
        input=file_content,
        cwd=temp_dir,
        capture_output=True,
        text=True,
    )

    assert (
        result.returncode == expected_return_code
    ), f"Executable failed with exit code {result.returncode}"
    if expected_return_code != 0:
        return

    try:
        result = subprocess.run(
            ["lli", intermediate], cwd=temp_dir, capture_output=True, text=True
        )
    except FileNotFoundError:
        print("Could not find lli tool.")
        assert False

    # Compare the output of the executable with the expected output
    assert (
        result.stdout == expected_output
    ), "Executable output does not match the expected output."
