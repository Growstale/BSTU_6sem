import datetime
from task_manager.manager import TaskManager


def run():
    manager = TaskManager()

    while True:
        print("\n1. Add Task\n2. Mark Done\n3. Delete\n4. Show Pending\n5. Show Overdue\n6. Exit")
        choice = input("Choose: ").strip()

        try:
            if choice == "1":
                title = input("Title: ")
                deadline_str = input("Deadline (YYYY-MM-DD): ")
                tags = input("Tags (comma separated): ").split(',')
                deadline = datetime.datetime.strptime(deadline_str, "%Y-%m-%d").date()
                task_id = manager.add_task(title, deadline, tags)
                print(f"Task added with ID {task_id}")
            elif choice == "2":
                task_id = int(input("Task ID: "))
                manager.mark_done(task_id)
                print("Marked as done.")
            elif choice == "3":
                task_id = int(input("Task ID: "))
                manager.delete_task(task_id)
                print("Deleted.")
            elif choice == "4":
                tasks = manager.get_pending_tasks()
                for t in tasks:
                    print(t)
            elif choice == "5":
                today = datetime.date.today()
                tasks = manager.get_overdue_tasks(today)
                for t in tasks:
                    print(t)
            elif choice == "6":
                break
            else:
                print("Invalid choice.")
        except Exception as e:
            print(f"Error: {e}")


if __name__ == "__main__":
    run()
