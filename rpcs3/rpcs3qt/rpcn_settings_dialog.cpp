#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegExpValidator>
#include <thread>

#include "rpcn_settings_dialog.h"
#include "Emu/NP/rpcn_config.h"
#include "Emu/NP/rpcn_client.h"

rpcn_settings_dialog::rpcn_settings_dialog(QWidget* parent)
    : QDialog(parent)
{
	setWindowTitle(tr("RPCN Configuration"));
	setObjectName("rpcn_settings_dialog");
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(QSize(700, 450));

	QVBoxLayout* vbox_global           = new QVBoxLayout();
	QHBoxLayout* hbox_labels_and_edits = new QHBoxLayout();
	QVBoxLayout* vbox_labels           = new QVBoxLayout();
	QVBoxLayout* vbox_edits            = new QVBoxLayout();
	QHBoxLayout* hbox_buttons          = new QHBoxLayout();

	QLabel* label_host = new QLabel(tr("Host:"));
	edit_host          = new QLineEdit();
	QLabel* label_npid = new QLabel(tr("NPID(username):"));
	edit_npid          = new QLineEdit();
	edit_npid->setMaxLength(16);
	edit_npid->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z0-9_\\-]*$"), this));
	QLabel* label_pass = new QLabel(tr("Password:"));
	edit_pass          = new QLineEdit();
	edit_pass->setEchoMode(QLineEdit::Password);

	QPushButton* btn_create = new QPushButton(tr("Create Account"), this);
	QPushButton* btn_ok     = new QPushButton(tr("Ok"), this);

	vbox_labels->addWidget(label_host);
	vbox_labels->addWidget(label_npid);
	vbox_labels->addWidget(label_pass);

	vbox_edits->addWidget(edit_host);
	vbox_edits->addWidget(edit_npid);
	vbox_edits->addWidget(edit_pass);

	hbox_buttons->addWidget(btn_create);
	hbox_buttons->addWidget(btn_ok);

	hbox_labels_and_edits->addLayout(vbox_labels);
	hbox_labels_and_edits->addLayout(vbox_edits);

	vbox_global->addLayout(hbox_labels_and_edits);
	vbox_global->addLayout(hbox_buttons);

	setLayout(vbox_global);

	connect(btn_ok, &QAbstractButton::clicked, [this]()
	{
		this->save_config();
		this->close();
	});
	connect(btn_create, &QAbstractButton::clicked, [this]() { this->create_account(); });

	g_cfg_rpcn.load();

	edit_host->setText(QString::fromStdString(g_cfg_rpcn.get_host()));
	edit_npid->setText(QString::fromStdString(g_cfg_rpcn.get_npid()));
	edit_pass->setText(QString::fromStdString(g_cfg_rpcn.get_password()));
}

bool rpcn_settings_dialog::save_config()
{
	const auto host     = edit_host->text().toStdString();
	const auto npid     = edit_npid->text().toStdString();
	const auto password = edit_pass->text().toStdString();

	auto validate = [](const std::string& input) -> bool
	{
		if (input.length() < 3 || input.length() > 16)
			return false;

		for (const auto c : input)
		{
			if (!std::isalnum(c) && c != '-' && c != '_')
				return false;
		}
		return true;
	};

	if (!host.size())
	{
		QMessageBox::critical(this, tr("Missing host"), tr("You need to enter a host for rpcn!"), QMessageBox::Ok);
		return false;
	}

	if (!npid.size() || !password.size())
	{
		QMessageBox::critical(this, tr("Wrong input"), tr("You need to enter a username and a password!"), QMessageBox::Ok);
		return false;
	}

	if (!validate(npid))
	{
		QMessageBox::critical(this, tr("Invalid character"), tr("NPID must be between 3 and 16 characters and can only contain '-', '_' or alphanumeric characters."), QMessageBox::Ok);
		return false;
	}

	g_cfg_rpcn.set_host(host);
	g_cfg_rpcn.set_npid(npid);
	g_cfg_rpcn.set_password(password);

	g_cfg_rpcn.save();

	return true;
}

bool rpcn_settings_dialog::create_account()
{
	// Validate and save
	if (!save_config())
		return false;

	const auto rpcn = std::make_shared<rpcn_client>(true);

	const auto host        = g_cfg_rpcn.host.to_string();
	const auto npid        = g_cfg_rpcn.npid.to_string();
	const auto online_name = npid;
	const auto avatar_url  = "https://i.imgur.com/AfWIyQP.jpg";
	const auto password    = g_cfg_rpcn.password.to_string();

	std::thread(
		[](const std::shared_ptr<rpcn_client> rpcn)
		{
			while (rpcn.use_count() != 1)
				rpcn->manage_connection();

			rpcn->disconnect();
		},
		rpcn)
		.detach();

	if (!rpcn->connect(host))
	{
		QMessageBox::critical(this, tr("Error Connecting"), tr("Failed to connect to RPCN server"), QMessageBox::Ok);
		rpcn->abort();
		return false;
	}

	if (!rpcn->create_user(npid, password, online_name, avatar_url))
	{
		QMessageBox::critical(this, tr("Error Creating Account"), tr("Failed to create the account (username exists?)"), QMessageBox::Ok);
		rpcn->abort();
		return false;
	}

	QMessageBox::information(this, tr("Account created!"), tr("Your account has been created successfully!"), QMessageBox::Ok);
	rpcn->abort();
	return true;
}
