#ifndef QUERIES_H
#define QUERIES_H

#define GET_MESSAGES_QUERY "SELECT *\
					  FROM (SELECT m.status, m.message_id, m.from_username,\
					  		m.message_text, m.send_datetime, m.chat_id \
							FROM %s c\
							INNER JOIN %s m\
							ON c.chat_id = m.chat_id\
							WHERE m.chat_id=%s AND m.loaded_by NOT LIKE '%%%s%%' \
							ORDER BY m.message_id DESC)\
					  ORDER BY message_id"

#define GET_CHATS_QUERY "SELECT c.status, \
						  c.chat_id,\
						c.chat_name,\
						CASE MAX_MID IS NULL\
							WHEN 1 THEN 'You' || ' @ ' || '...' || '@' || '...' \
							ELSE (SELECT m3.from_username || '@' || m3.message_text || '@' || m3.status FROM %s m3 WHERE m3.message_id=MAX_MID)\
						END MESSAGE_DATA\
						,\
						c.chat_members\
					FROM (SELECT *, (SELECT max(message_id) FROM %s m WHERE m.chat_id = c2.chat_id) AS MAX_MID FROM %s c2) c \
					WHERE c.chat_members LIKE '%%%s%%' AND c.loaded_by NOT LIKE '%%%s%%'"

#define CREATE_MESSAGE_QUERY "INSERT INTO %s(from_username, message_text, send_datetime, chat_id) VALUES('%s', '%s', %s, %s)"
#define EDIT_MESSAGE_QUERY "UPDATE %s SET message_text='%s', loaded_by='', status='%s' WHERE message_id=%s"
#define DELETE_MESSAGE_QUERY "UPDATE %s SET loaded_by='', status='%s' WHERE message_id=%s"

#define CLEANUP_DELETED_MESSAGES_QUERY "DELETE FROM %s WHERE status='%s' AND from_username='%s'"
#define CLEANUP_DELETED_CHATS_QUERY  "DELETE FROM %s WHERE status='%s' AND chat_members LIKE '%%%s%%'"

#define GO_ONLINE_QUERY "UPDATE %s SET online='online' WHERE username='%s'"
#define GO_OFFLINE_QUERY "UPDATE %s SET online='offline' WHERE username='%s'"

#define RESET_LOADING_STATUS "UPDATE %s SET loaded_by=REPLACE(loaded_by, '%s', '') WHERE loaded_by LIKE '%%%s%%' AND status='%s'"
#define RESET_CHAT_MESSAGES_LOADING_STATUS  "UPDATE %s SET loaded_by=REPLACE(loaded_by, '%s', '') WHERE chat_id=%s"

#endif

