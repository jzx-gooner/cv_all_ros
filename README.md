# cv_all_ros
detection_segmentation_classification_in_one_ros_node

# todo
# 需要订阅xarm_ros的消息，放到消息里面
# 需要写一个服务，等待调用，根据服务类别不同，处理不同的信息
# 服务根据标志位不同  0.计算出正对位置，返回正对位置，此时模型只启用detection  1.计算精准位置
# 服务0：img->detection->初始位置1->返回初始位置1-->返回 机械臂移动到第二个位置
# 服务1：img->detection->segmentation->classification = 返回状态，最终位置
# 服务2：按钮
# 服务3：开关
# 服务4：。。。


# how to
# 图像callback函数里面进入 switch case
# {NORMAL，STEP1，BUTTON...}
# normal下，一直publish，写入detection数据
# 服务更改标注位，如果标志位变了，进入按钮。开关模式，
# 进入开关模式，先移动到那个位置
