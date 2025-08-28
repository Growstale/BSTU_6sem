
using Microsoft.SqlServer.Server;
using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Linq;
using System.Net.Mail;
using System.Net;
using System.Text;
using System.Threading.Tasks;
public class SqlClrFunctions
{
    // 1. Хранимая процедура: Отправка Email при изменении прав на объект
    [SqlProcedure]
    public static void SendEmailOnPermissionChange(SqlString recipient, SqlString subject, SqlString body)
    {
        try
        {
            using (SmtpClient client = new SmtpClient("smtp.gmail.com", 587))
            {
                client.Credentials = new NetworkCredential("cleo2005254@gmail.com", "vsba gybp ayza vper\r\n");
                client.EnableSsl = true;

                MailMessage message = new MailMessage();
                message.From = new MailAddress("cleo2005254@gmail.com");
                message.To.Add(recipient.Value);
                message.Subject = subject.Value;
                message.Body = body.Value;

                client.Send(message);
            }
        }
        catch (Exception ex)
        {
            throw new Exception("Error while sending email: " + ex.ToString());
        }
    }
    public static void Main()
    {
        SendEmailOnPermissionChange("vodchytsanastasiya@gmail.com", "Тема письма", "Текст письма");
    }
}
