# 振动频域分析上位机 - PowerShell依赖安装脚本

Write-Host "========================================" -ForegroundColor Green
Write-Host "振动频域分析上位机 - 依赖安装脚本" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

Write-Host "正在检查Python环境..." -ForegroundColor Yellow

# 检查Python是否安装
try {
    $pythonVersion = python --version 2>&1
    Write-Host "Python版本: $pythonVersion" -ForegroundColor Green
} catch {
    Write-Host "错误: 未找到Python，请先安装Python 3.7+" -ForegroundColor Red
    Read-Host "按任意键退出"
    exit 1
}

Write-Host ""
Write-Host "正在安装Python依赖包..." -ForegroundColor Yellow

# 安装依赖包
$packages = @("PyQt5", "pyqtgraph", "pyserial", "numpy")

foreach ($package in $packages) {
    Write-Host "安装 $package..." -ForegroundColor Cyan
    try {
        pip install $package
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ $package 安装成功" -ForegroundColor Green
        } else {
            Write-Host "✗ $package 安装失败" -ForegroundColor Red
        }
    } catch {
        Write-Host "✗ $package 安装出错: $_" -ForegroundColor Red
    }
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Green
Write-Host "安装完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "使用方法:" -ForegroundColor Yellow
Write-Host "1. 运行: python vibration_analyzer.py" -ForegroundColor White
Write-Host "2. 选择STM32对应的串口" -ForegroundColor White
Write-Host "3. 点击'连接'按钮" -ForegroundColor White
Write-Host "4. 观察实时频域图表" -ForegroundColor White
Write-Host ""

Read-Host "按任意键退出"
