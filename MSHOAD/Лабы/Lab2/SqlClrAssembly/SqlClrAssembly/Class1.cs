using System;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Net;
using System.Net.Mail;
using Microsoft.SqlServer.Server;
using System.Text.RegularExpressions;
using System.IO;

public class SqlClrFunctions
{
    [SqlFunction(IsDeterministic = true, IsPrecise = true)]
    public static SqlString PhoneNumberToString(PhoneNumber phone)
    {
        return phone.IsNull ? SqlString.Null : new SqlString(phone.ToString());
    }

    [SqlProcedure]
    public static void SendEmailOnPermissionChange(SqlString recipient, SqlString subject, SqlString body)
    {
        try
        {
            using (SmtpClient client = new SmtpClient("smtp.gmail.com", 587))
            {
                client.Credentials = new NetworkCredential("cleo2005254@gmail.com", "");
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
            throw new Exception("Error while sending email: " + ex.Message);
        }
    }
}

[Serializable]
[SqlUserDefinedType(Format.UserDefined, MaxByteSize = 50)]
public struct PhoneNumber : INullable, IBinarySerialize
{
    private string _number;
    private bool _isNull;

    public bool IsNull => _isNull;
    public static PhoneNumber Null => new PhoneNumber { _isNull = true };

    public override string ToString() => _isNull ? "NULL" : _number;

    public static PhoneNumber Parse(SqlString s)
    {
        if (s.IsNull || string.IsNullOrWhiteSpace(s.Value))
            return Null;

        string pattern = @"^\+375\(?(25|44|29|33|17)\)?\d{7}$";
        if (!Regex.IsMatch(s.Value, pattern))
            throw new FormatException("Некорректный формат номера телефона!");

        return new PhoneNumber { _number = s.Value, _isNull = false };
    }

    // Реализация сериализации, записывает состояние объекта в бинарный поток
    public void Write(BinaryWriter writer)
    {
        writer.Write(_isNull);
        writer.Write(_isNull ? string.Empty : _number);
    }

    // Реализация десериализации, читает данные из бинарного потока
    public void Read(BinaryReader reader)
    {
        _isNull = reader.ReadBoolean();
        _number = _isNull ? null : reader.ReadString();
    }
}