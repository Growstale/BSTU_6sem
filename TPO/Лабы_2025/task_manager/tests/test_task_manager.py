import unittest
from datetime import date, timedelta
from task_manager.manager import TaskManager, Task
from task_manager.exceptions import TaskNotFoundError
from unittest.mock import patch


class TestTaskManager(unittest.TestCase):
    # inherited from unittest.TestCase, which gives access to all verification functions
    def setUp(self):
        # This method is run before each test. It creates a fresh instance of Task Manager
        # so that each test is isolated from the rest (it does not use tasks from other tests).
        self.manager = TaskManager()

    def test_add_task_valid(self):
        task_id = self.manager.add_task("Test", date.today(), ["work"])
        self.assertEqual(task_id, 0)
        self.assertIn(task_id, self.manager.tasks)

    def test_add_task_invalid_title(self):
        with self.assertRaises(ValueError) as context:
            self.manager.add_task("   ", date.today())
        self.assertEqual(str(context.exception), "Title cannot be empty")

    def test_mark_task_done(self):
        task_id = self.manager.add_task("Task", date.today())
        self.manager.mark_done(task_id)
        self.assertTrue(self.manager.tasks[task_id].done)

    def test_mark_task_done_invalid_id(self):
        with self.assertRaises(TaskNotFoundError) as context:
            self.manager.mark_done(99)
        self.assertEqual(str(context.exception), "Task ID not found")

    def test_delete_task(self):
        task_id = self.manager.add_task("Delete me", date.today())
        self.manager.delete_task(task_id)
        self.assertNotIn(task_id, self.manager.tasks)

    def test_delete_task_invalid_id(self):
        with self.assertRaises(TaskNotFoundError) as context:
            self.manager.delete_task(123)
        self.assertEqual(str(context.exception), "Task ID not found")

    def test_get_pending_tasks(self):
        self.manager.add_task("A", date.today())
        task_id = self.manager.add_task("B", date.today())
        self.manager.mark_done(task_id)
        pending = self.manager.get_pending_tasks()
        self.assertEqual(len(pending), 1)

    def test_get_tasks_by_tag(self):
        self.manager.add_task("A", date.today(), ["home"])
        self.manager.add_task("B", date.today(), ["work"])
        tasks = self.manager.get_tasks_by_tag("home")
        self.assertEqual(len(tasks), 1)
        self.assertEqual(tasks[0].title, "A")

    def test_get_overdue_tasks(self):
        yesterday = date.today() - timedelta(days=1)
        self.manager.add_task("Overdue", yesterday)
        self.manager.add_task("Today", date.today())
        overdue = self.manager.get_overdue_tasks(date.today())
        self.assertEqual(len(overdue), 1)

    def test_str_representation(self):
        task = Task("Do homework", date.today(), ["study"])
        self.assertIn("Do homework", str(task))

    def test_task_mark_done_changes_str(self):
        task = Task("Test", date.today())
        task.mark_done()
        self.assertIn("✓", str(task))

    def test_add_task_with_no_tags(self):
        task_id = self.manager.add_task("No tags", date.today(), None)
        task = self.manager.tasks[task_id]
        self.assertEqual(task.tags, [])

    def test_multiple_tasks_increment_id(self):
        self.manager.add_task("T1", date.today())
        task_id = self.manager.add_task("T2", date.today())
        self.assertEqual(task_id, 1)

    @patch('task_manager.manager.datetime.date')
    def test_get_overdue_tasks_with_mocked_today(self, mock_date):
        mock_date.today.return_value = date(2025, 6, 10)
        manager = TaskManager()
        manager.add_task("Old task", date(2025, 6, 5))
        overdue = manager.get_overdue_tasks(mock_date.today())
        self.assertEqual(len(overdue), 1)


if __name__ == "__main__":
    unittest.main()
