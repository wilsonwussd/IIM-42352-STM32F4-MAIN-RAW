@echo off
echo ========================================
echo 振动频域分析上位机 - 依赖安装脚本
echo ========================================

echo 正在安装Python依赖包...

pip install PyQt5
pip install pyqtgraph
pip install pyserial
pip install numpy

echo.
echo ========================================
echo 安装完成！
echo ========================================
echo.
echo 使用方法:
echo 1. 运行: python vibration_analyzer.py
echo 2. 选择STM32对应的串口
echo 3. 点击"连接"按钮
echo 4. 观察实时频域图表
echo.
pause
