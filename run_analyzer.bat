@echo off
echo ========================================
echo 专业振动分析仪 v3.1 - 启动脚本
echo ========================================
echo.
echo 新界面设计: 频域+时域同步显示
echo - 上方图表: 频域分析 (振动频谱)
echo - 下方图表: 时域分析 (三轴加速度实时波形)
echo - 同一界面: 方便对比分析
echo.
echo 正在启动上位机程序...
python vibration_analyzer_chinese.py
echo.
echo 程序已退出
pause
