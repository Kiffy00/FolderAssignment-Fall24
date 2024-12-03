#ifndef TASKCONFIG_H
#define TASKCONFIG_H

struct TaskConfig {
    enum class TaskType : uint8_t {
        Task1_Subtask2,
        Task1_Subtask3,
        Task1_Subtask4,
        Task2
    } taskType;
};

#endif