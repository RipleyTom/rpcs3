#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

class rpcn_settings_dialog : public QDialog
{
	Q_OBJECT
public:
	static rpcn_settings_dialog* get_dlg(QWidget* parent);

private:
	rpcn_settings_dialog(QWidget *parent = nullptr);
	static inline rpcn_settings_dialog* inst = nullptr;
};

class rpcn_account_dialog : public QDialog
{
	Q_OBJECT

public:
	static rpcn_account_dialog* get_dlg(QWidget* parent);

	bool save_config();
	bool create_account();

protected:
	
	QLineEdit *m_edit_host, *m_edit_npid, *m_edit_token;
private:
	rpcn_account_dialog(QWidget* parent = nullptr);
	static inline rpcn_account_dialog* inst = nullptr;
};

class rpcn_friends_dialog : public QDialog
{
	Q_OBJECT

public:
	static rpcn_friends_dialog* get_dlg(QWidget* parent);

protected:
	// list of friends
	QListWidget *lst_friends = nullptr;
	// list of friend requests sent to the current user
	QListWidget *lst_requests = nullptr;
	// list of people blocked by the user
	QListWidget *lst_blocks = nullptr;

private:
	rpcn_friends_dialog(QWidget* parent = nullptr);
	static inline rpcn_friends_dialog* inst = nullptr;

};
