#include <QDialog>
#include <QLineEdit>

class rpcn_settings_dialog : public QDialog
{
	Q_OBJECT

public:
	rpcn_settings_dialog(QWidget* parent = nullptr);

	bool save_config();
	bool create_account();

protected:
	QLineEdit *edit_host, *edit_npid, *edit_pass;
};
