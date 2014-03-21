<?php
/**
 * uc_shop_order_history($user)
 */
function uc_shop_order_history($user) {
  drupal_set_title(t('My order history'));

  $header = array(
    array('data' => t('店铺'), 'field' => 'dm.sitename'),
    array('data' => t('Date'), 'field' => 'o.created', 'sort' => 'desc'),
    array('data' => t('Order #'), 'field' => 'o.order_id'),
    array('data' => t('Status'), 'field' => 'os.title'),
    array('data' => t('Products'), 'field' => 'products'),
    array('data' => t('Total'), 'field' => 'o.order_total')
  );

  $rows = array();

  $context = array(
    'revision' => 'themed-original',
    'type' => 'amount',
  );

  // $result = pager_query("SELECT o.order_id, o.uid, o.created, os.title, SUM(op.qty) AS products, o.order_total AS total, o.order_status FROM {uc_orders} AS o LEFT JOIN {uc_order_statuses} AS os ON o.order_status = os.order_status_id LEFT JOIN {uc_order_products} AS op ON o.order_id = op.order_id WHERE o.uid = %d AND o.order_status IN ". uc_order_status_list('general', TRUE) ." GROUP BY o.order_id, o.uid, o.created, os.title, o.order_total, o.order_status". tablesort_sql($header), 20, 0, "SELECT COUNT(*) FROM {uc_orders} WHERE uid = %d AND order_status NOT IN ". uc_order_status_list('specific', TRUE), $user->uid);
  $result = pager_query("SELECT dm.sitename, o.order_id, o.uid, o.created, os.title, SUM(op.qty) AS products, o.order_total AS total, o.order_status 
    FROM {uc_orders} AS o LEFT JOIN {uc_order_statuses} AS os ON o.order_status = os.order_status_id 
    LEFT JOIN {uc_order_products} AS op ON o.order_id = op.order_id
    LEFT JOIN {uc_domain_orders} AS do ON o.order_id = do.order_id
    LEFT JOIN {domain} AS dm ON dm.domain_id = do.domain_id
    WHERE o.uid = %d AND o.order_status IN ". uc_order_status_list('general', TRUE) ." 
    GROUP BY o.order_id, o.uid, o.created, os.title, o.order_total, o.order_status". tablesort_sql($header), 20, 0,
    "SELECT COUNT(*) FROM {uc_orders} WHERE uid = %d AND order_status NOT IN ". uc_order_status_list('specific', TRUE), $user->uid);
  
  // Build a table based on the customer's orders.
  while ($order = db_fetch_object($result)) {
    $context['subject'] = array('order' => $order);

    $link = l($order->order_id, 'user/'. $user->uid .'/order/'. $order->order_id);

    if (user_access('view all orders')) {
      $link .= '<span class="order-admin-icons">'. uc_order_actions($order, TRUE) .'</span>';
    }

    $rows[] = array(
      array('data' => check_plain($order->sitename)),
      array('data' => format_date($order->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y'))),
      array('data' => $link),
      array('data' => check_plain($order->title)),
      array('data' => (!is_null($order->products) ? $order->products : 0), 'align' => 'center'),
      array('data' => uc_price($order->total, $context), 'align' => 'right'),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No orders available.'), 'colspan' => 5));
  }

  return theme('table', $header, $rows, array('class' => 'uc-order-history'))
       . theme('pager', NULL, 20, 0);
  
}