#include "../includes/sendEmail.h"

string host("smtp.gmail.com");
UInt16 port = 587;
string password = "vymatdrvasbcdwpw";
string sender = "networkSecMiniProject@gmail.com";
string receiver = "networkSecMiniProject@gmail.com";

bool sendEmail(string subject, string content) {

	Poco::Net::SecureSMTPClientSession session(host, port);
	session.open();
	Poco::Net::initializeSSL();
	SharedPtr<InvalidCertificateHandler> ptrHandler = new AcceptCertificateHandler(false);
	Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "", Context::VERIFY_NONE, 9, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);
	try
	{
		session.login();
		
		if (session.startTLS(ptrContext))
		{
			session.login(SMTPClientSession::AUTH_LOGIN, sender, password);
			Poco::Net::MailMessage message;
			Poco::Net::MailRecipient recipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, receiver);
			message.addRecipient(recipient);
			message.setSubject(subject);
			message.setContent(content);
			session.sendMessage(message);

		}
		session.close();
		Poco::Net::uninitializeSSL();
		return true;
	}
	catch (Poco::Exception& e)
	{
		session.close();
		Poco::Net::uninitializeSSL();
		return false;
	}

}