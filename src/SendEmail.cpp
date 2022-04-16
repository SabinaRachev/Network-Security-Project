#include "../includes/sendEmail.h"

string host("smtp.gmail.com");
UInt16 port = 587;
string password = "rddfwfqnnwitqglt";
string sender = "networkSecMiniProject@gmail.com";
string receiver = "networkSecMiniProject@gmail.com";

bool sendEmail(string subject,string content) {

	
	SecureSMTPClientSession session(host, port);
	session.open();
	Poco::Net::initializeSSL();
	SharedPtr<InvalidCertificateHandler> ptrHandler = new AcceptCertificateHandler(false);
	Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "", Context::VERIFY_RELAXED);
	SSLManager::instance().initializeClient(0, ptrHandler, ptrContext);

	try
	{
		// SSL
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
		cout << "Message was sent!\n";
		return true;
	}
	catch (Poco::Exception& e)
	{
		cout << e.message() << endl;
		session.close();
		Poco::Net::uninitializeSSL();
		return false;
	}
}