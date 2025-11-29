# GitHub Repository Cleanup Script
# Run this in PowerShell to organize the repository for GitHub

Write-Host "`n=== HiTechnic TETRIX Arduino Library - Cleanup Script ===" -ForegroundColor Cyan
Write-Host "This will organize the repository structure for GitHub`n"

# Create new directory structure
Write-Host "Creating directory structure..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path "src" -Force | Out-Null
New-Item -ItemType Directory -Path "examples\Motor" -Force | Out-Null
New-Item -ItemType Directory -Path "examples\Servo" -Force | Out-Null
New-Item -ItemType Directory -Path "examples\Diagnostic" -Force | Out-Null
New-Item -ItemType Directory -Path "docs\specs" -Force | Out-Null
New-Item -ItemType Directory -Path "extras" -Force | Out-Null

# Move source files to src/
Write-Host "Moving library source files to src/..." -ForegroundColor Yellow
Move-Item "HiTechnicMotor.h" "src\" -Force -ErrorAction SilentlyContinue
Move-Item "HiTechnicMotor.cpp" "src\" -Force -ErrorAction SilentlyContinue
Move-Item "HiTechnicServo.h" "src\" -Force -ErrorAction SilentlyContinue
Move-Item "HiTechnicServo.cpp" "src\" -Force -ErrorAction SilentlyContinue
Move-Item "SoftwareI2C.h" "src\" -Force -ErrorAction SilentlyContinue
Move-Item "SoftwareI2C.cpp" "src\" -Force -ErrorAction SilentlyContinue

# Move motor examples
Write-Host "Organizing motor examples..." -ForegroundColor Yellow
$motorExamples = @("SixMotorsWorking", "BasicMotorControl", "DualMotorControl", "EncoderReading", "PositionControl")
foreach ($ex in $motorExamples) {
    if (Test-Path "examples\$ex") {
        Move-Item "examples\$ex" "examples\Motor\" -Force -ErrorAction SilentlyContinue
    }
}

# Move servo examples
Write-Host "Organizing servo examples..." -ForegroundColor Yellow
$servoExamples = @("BasicServoControl", "MultipleServos")
foreach ($ex in $servoExamples) {
    if (Test-Path "examples\$ex") {
        Move-Item "examples\$ex" "examples\Servo\" -Force -ErrorAction SilentlyContinue
    }
}

# Move diagnostic examples
Write-Host "Organizing diagnostic examples..." -ForegroundColor Yellow
$diagnosticExamples = @("I2CScanner", "DaisyChainAddressTest")
foreach ($ex in $diagnosticExamples) {
    if (Test-Path "examples\$ex") {
        Move-Item "examples\$ex" "examples\Diagnostic\" -Force -ErrorAction SilentlyContinue
    }
}

# Move documentation
Write-Host "Organizing documentation..." -ForegroundColor Yellow
if (Test-Path "WIRING.md") { Move-Item "WIRING.md" "docs\" -Force }
if (Test-Path "DAISY_CHAIN_ADDRESSING.md") { Move-Item "DAISY_CHAIN_ADDRESSING.md" "docs\" -Force }
if (Test-Path "SUCCESS.md") { Move-Item "SUCCESS.md" "docs\SPECIFICATIONS.md" -Force }
if (Test-Path "*.pdf") { Move-Item "*.pdf" "docs\specs\" -Force -ErrorAction SilentlyContinue }

# Move extras
Write-Host "Moving extras..." -ForegroundColor Yellow
if (Test-Path "extract_pdf.py") { Move-Item "extract_pdf.py" "extras\" -Force }

# Replace README
Write-Host "Updating README..." -ForegroundColor Yellow
if (Test-Path "README_NEW.md") {
    if (Test-Path "README.md") { Remove-Item "README.md" -Force }
    Move-Item "README_NEW.md" "README.md" -Force
}

# Remove unwanted examples
Write-Host "Removing experimental/debugging examples..." -ForegroundColor Yellow
$removeExamples = @(
    "AddressDiagnostic", "AdvancedAddressChange", "AllControllersTest",
    "AnalogDetectionTest", "AnalogSelector", "ChangeControllerAddress",
    "CorrectAddressScan", "DaisyChainAddressScan", "DaisyChainExplorer",
    "DaisyChainMotorTest", "DeviceSlotDetector", "EnhancedScanner",
    "ExtendedAddressScan", "FindAllControllers", "FullI2CScan",
    "HiddenCommandExplorer", "IndividualSelectionTest", "MotorControl_0x05",
    "MotorDiagnostics", "MultiController", "QuickScan", "RawI2CTest",
    "RegisterTest", "SequentialMotorTest", "SingleController_0x05",
    "SoftwareI2CTest", "SoftwareSelector", "TestBothMotors",
    "TestBothMotors_0x05", "ThreeControllers", "ThreeControllersWorking",
    "TransactionVerification"
)

foreach ($ex in $removeExamples) {
    if (Test-Path "examples\$ex") {
        Remove-Item "examples\$ex" -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Remove unwanted documentation
Write-Host "Removing redundant documentation..." -ForegroundColor Yellow
$removeDocs = @(
    "5V_DETECTION_WIRING.md", "ANALOG_SELECTOR_WIRING.md",
    "MULTI_CONTROLLER_SETUP.md", "PROJECT_SUMMARY.md",
    "QUICK_REFERENCE.md", "SOLUTION.md", "STRUCTURE.md",
    "CLEANUP_PLAN.md"
)

foreach ($doc in $removeDocs) {
    if (Test-Path $doc) {
        Remove-Item $doc -Force -ErrorAction SilentlyContinue
    }
}

# Remove build artifacts and temporary files
Write-Host "Removing build artifacts..." -ForegroundColor Yellow
if (Test-Path "arduino-cli.exe") { Remove-Item "arduino-cli.exe" -Force }
Get-ChildItem -Filter "*.txt" | Where-Object { $_.Name -notin @("LICENSE.txt", "keywords.txt") } | Remove-Item -Force -ErrorAction SilentlyContinue

# Create README files in example categories
Write-Host "Creating example category READMEs..." -ForegroundColor Yellow

@"
# Motor Examples

These examples demonstrate control of HiTechnic TETRIX DC Motor Controllers.

## Examples

- **SixMotorsWorking** - Complete demo controlling 6 motors independently (recommended)
- **BasicMotorControl** - Simple single motor control
- **DualMotorControl** - Control both motors on one controller
- **EncoderReading** - Read encoder values from motors
- **PositionControl** - Move motors to specific positions using encoders
"@ | Out-File "examples\Motor\README.md" -Encoding UTF8

@"
# Servo Examples

These examples demonstrate control of HiTechnic TETRIX Servo Controllers.

## Examples

- **BasicServoControl** - Control a single servo motor
- **MultipleServos** - Control all 6 servos on a servo controller
"@ | Out-File "examples\Servo\README.md" -Encoding UTF8

@"
# Diagnostic Examples

These examples help verify your hardware setup and troubleshoot issues.

## Examples

- **I2CScanner** - Scan the I2C bus to find connected controllers
- **DaisyChainAddressTest** - Verify daisy chain addressing is working correctly
"@ | Out-File "examples\Diagnostic\README.md" -Encoding UTF8

# Summary
Write-Host "`n=== Cleanup Complete! ===" -ForegroundColor Green
Write-Host "`nRepository structure:"
Write-Host "  ✓ Library source files moved to src/"
Write-Host "  ✓ Examples organized into Motor/Servo/Diagnostic categories"
Write-Host "  ✓ Documentation moved to docs/"
Write-Host "  ✓ Specifications moved to docs/specs/"
Write-Host "  ✓ Experimental files removed"
Write-Host "  ✓ Build artifacts cleaned up"
Write-Host "`nNext steps:"
Write-Host "  1. Review the cleaned structure"
Write-Host "  2. Update library.properties with your info"
Write-Host "  3. Update README.md with your GitHub URL"
Write-Host "  4. Initialize git: git init"
Write-Host "  5. Add files: git add ."
Write-Host "  6. Commit: git commit -m 'Initial commit'"
Write-Host "  7. Push to GitHub"
Write-Host "`nRepository is ready for GitHub!" -ForegroundColor Cyan
