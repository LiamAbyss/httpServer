#include "sendMail.h"


using namespace EASendMailObjLib;
using namespace std;

void sendMail(Mail_ mail)
{
	TCHAR* sender = (TCHAR*)mail.sender.c_str();
	TCHAR* recipient = (TCHAR*)mail.recipient.c_str();
	TCHAR* password = (TCHAR*)mail.password.c_str();
	::CoInitialize(NULL);
	IMailPtr oSmtp = NULL;
	oSmtp.CreateInstance("EASendMailObj.Mail");
	oSmtp->LicenseCode = _T("TryIt");

	// Set your sender email address
	oSmtp->FromAddr = _T(sender);

	// Add recipient email address
	oSmtp->AddRecipientEx(_T(recipient), 0);

	// Set email subject
	oSmtp->Subject = _T(mail.subject.c_str());

	// Set email body
	oSmtp->BodyText = _T(mail.body.c_str());

	// Your SMTP server address
	oSmtp->ServerAddr = _T("smtp.live.com");

	// User and password for ESMTP authentication, if your server doesn't
	// require User authentication, please remove the following codes.
	oSmtp->UserName = _T(sender);
	oSmtp->Password = _T(password);

	// If your smtp server requires SSL connection, please add this line
	int retour;
	oSmtp->SSL_init(); if (oSmtp->SendMail() == 0)
	{
		retour = 1;
		//_tprintf(_T("Emails envoyes avec succes !\n"));
	}
	else
	{
		//_tprintf(_T("Echec lors de l'envoi des mails du a l'erreur suivante : %s\r\n"),
			//(const TCHAR*)oSmtp->GetLastErrDescription());
		retour = 0;
	}
	if (oSmtp != NULL)
		oSmtp.Release();
}

void Mail_::send()
{
	sendMail(*this);
}
