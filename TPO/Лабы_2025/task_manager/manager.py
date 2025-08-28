import datetime
from typing import List, Dict
from .exceptions import TaskNotFoundError


class Task:
    def __init__(self, title: str, deadline: datetime.date, tags: List[str] = None):
        # create new task
        if not title.strip():
            raise ValueError("Title cannot be empty")
        self.title = title
        self.deadline = deadline
        self.tags = tags or []
        self.done = False

    def mark_done(self):
        # task is done
        self.done = True

    def __str__(self):
        # task as string
        status = "✓" if self.done else "✗"
        return f"[{status}] {self.title} (due: {self.deadline}, tags: {', '.join(self.tags)})"


class TaskManager:
    def __init__(self):
        self.tasks: Dict[int, Task] = {}
        self.counter = 0

    def add_task(self, title: str, deadline: datetime.date, tags: List[str] = None) -> int:
        task = Task(title, deadline, tags)
        task_id = self.counter
        self.tasks[task_id] = task
        self.counter += 1
        return task_id

    def mark_done(self, task_id: int):
        if task_id not in self.tasks:
            raise TaskNotFoundError("Task ID not found")
        self.tasks[task_id].mark_done()

    def delete_task(self, task_id: int):
        if task_id not in self.tasks:
            raise TaskNotFoundError("Task ID not found")
        del self.tasks[task_id]

    def get_pending_tasks(self) -> List[Task]:
        return [task for task in self.tasks.values() if not task.done]

    def get_tasks_by_tag(self, tag: str) -> List[Task]:
        return [task for task in self.tasks.values() if tag in task.tags]

    def get_overdue_tasks(self, today: datetime.date) -> List[Task]:
        return [task for task in self.tasks.values() if task.deadline < today and not task.done]
