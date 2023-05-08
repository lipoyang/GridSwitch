/********** UIの要素 ***********/
// ボタン
const btn_connect  = document.getElementById('btn_connect'); // 接続
const btn_disconnect = document.getElementById('btn_disconnect');  // 切断
const btn_prev     = document.getElementById('btn_prev');   // 前
const btn_next     = document.getElementById('btn_next');   // 次
const btn_reload   = document.getElementById('btn_reload'); // Reload
const btn_save     = document.getElementById('btn_save');   // Save
const btn_import   = document.getElementById('btn_import'); // Import
const btn_export   = document.getElementById('btn_export'); // Export
// テキスト
const text_connect = document.getElementById('text_connect'); // 接続時のメッセージ
const text_layer    = document.getElementById('text_layer');  // レイヤー数表示
const text_name    = document.getElementById('text_name');    // 名前
const text_key = [                      // キー割り当て
  document.getElementById('text_key1'),
  document.getElementById('text_key2'),
  document.getElementById('text_key3'),
  document.getElementById('text_key4'),
  document.getElementById('text_key5'),
  document.getElementById('text_key6'),
  document.getElementById('text_key7'),
  document.getElementById('text_key8'),
  document.getElementById('text_key9'),
  document.getElementById('text_key10')
];
// チェックボックス
const check_enabled = document.getElementById('check_enabled'); // 使用する
const check_alt = [                      // Alt
  document.getElementById('check_a1'),
  document.getElementById('check_a2'),
  document.getElementById('check_a3'),
  document.getElementById('check_a4'),
  document.getElementById('check_a5'),
  document.getElementById('check_a6'),
  document.getElementById('check_a7'),
  document.getElementById('check_a8'),
  document.getElementById('check_a9'),
  document.getElementById('check_a10')
];
const check_ctrl = [                      // Ctrl
  document.getElementById('check_c1'),
  document.getElementById('check_c2'),
  document.getElementById('check_c3'),
  document.getElementById('check_c4'),
  document.getElementById('check_c5'),
  document.getElementById('check_c6'),
  document.getElementById('check_c7'),
  document.getElementById('check_c8'),
  document.getElementById('check_c9'),
  document.getElementById('check_c10')
];
const check_shift = [                      // Shift
  document.getElementById('check_s1'),
  document.getElementById('check_s2'),
  document.getElementById('check_s3'),
  document.getElementById('check_s4'),
  document.getElementById('check_s5'),
  document.getElementById('check_s6'),
  document.getElementById('check_s7'),
  document.getElementById('check_s8'),
  document.getElementById('check_s9'),
  document.getElementById('check_s10')
];
// カラーピッカー
const color_led = document.getElementById('color_led'); // LED表示色
// 表示領域
const panel_connect = document.getElementById('panel_connect'); // 接続画面
const panel_main    = document.getElementById('panel_main');    // メイン画面

/********** シリアル通信の定数・変数 ***********/
let comPort = null; // COMポート
const COM_READ     = 0x10; // READコマンド
const COM_WRITE    = 0x20; // WRITEコマンド
const ERR_CHECKSUM = 0x80; // エラー：チェックサムエラー
const ERR_COMMAND  = 0x81; // エラー：不正なコマンド
const ERR_PARAM    = 0x82; // エラー：不正なパラメータ

/********** キーボード設定データの定数・変数 ***********/
const PAGE_MAX = 8; // 最大レイヤー数
let layerNum = 0;   // 現在のレイヤー番号

const KEY_MAX = 10; // 物理キーの数

// キー修飾のビットマスク
const ALT_MASK   = 0x04;
const CTRL_MASK  = 0x02;
const SHIFT_MASK = 0x01;

// キーマップの配列[レイヤー番号]
let keyMaps = new Array(); 

// キーマップオブジェクトのコンストラクタ
function KeyMap(enabled, name, led, modifiers, keyCodes){
  this.Enabled   = enabled;    // 使用する？
  this.Name      = name;       // 名前
  this.Led       = led;        // LED表示色
  this.Modifiers = modifiers;  // キー修飾の配列[キー番号]
  this.KeyCodes  = keyCodes;   // キー割当の配列[キー番号]
}

/********** UIのイベントハンドラ ***********/
// 「接続」ボタン
btn_connect.addEventListener('click', async function () {
  try {
    // COMポートを開く
    comPort = await navigator.serial.requestPort();
    await comPort.open({ baudRate: 115200 });
    
    // キーマップ情報の読み出し
    const result = await loadKeyMap();

    // 画面表示切替
    if(result){
      panel_connect.style.display = "none";
      panel_main.style.display = "block";
    }
  } catch (error) {
    error_toast("ERROR: " + error)
    comPort = null;
  }
});

// 「切断」ボタン
btn_disconnect.addEventListener('click', async function (){
  if(comPort != null){
    await comPort.close();
  }
  // 画面表示切替
  text_connect.innerText = "";
  panel_main.style.display = "none";
  panel_connect.style.display = "block";
});

// 「前」ボタン
btn_prev.addEventListener('click', function (){
  // 現在のレイヤーの設定を記憶
  setKeyMap();
  // レイヤー移動
  layerNum--;
  if(layerNum < 0) layerNum += PAGE_MAX;
  showKeyMap();
});

// 「次」ボタン
btn_next.addEventListener('click', function (){
  // 現在のレイヤーの設定を記憶
  setKeyMap();
  // レイヤー移動
  layerNum++;
  if(layerNum >= PAGE_MAX) layerNum = 0;
  showKeyMap();
});

// 「Reload」ボタン
btn_reload.addEventListener('click', async function ()
{
  await loadKeyMap();
});

// 「Save」ボタン
btn_save.addEventListener('click', async function ()
{
  await saveKeyMap();
});

// 「Import」ボタン
btn_import.addEventListener('click', async function (){

  // ファイルを読み込む (PC版 Chrome, Edge, Opera が対応)
  const options = {
    types: [{
      description: "JSON File",
      accept: {'text/json': ['.json']},
    }],
  };
  const handleList = await window.showOpenFilePicker(options);
  const file = await handleList[0].getFile(); // 1個目のファイル
  const json = await file.text();

  // JSON文字列をキーマップのデータに変換
  keyMaps = JSON.parse(json);
  
  // 表示を更新
  layerNum = 0;
  showKeyMap();
});

// 「Export」ボタン
btn_export.addEventListener('click', async function (){

  // キーマップのデータをJSON文字列に変換
  const json = JSON.stringify(keyMaps);

  // ファイルに保存 (PC版 Chrome, Edge, Opera が対応)
  const options = {
    suggestedName: 'keymap',
    types: [{
      description: 'JSON file',
      accept: {'text/json': ['.json']},
    }],
  };
  const handle = await window.showSaveFilePicker(options);
  const writable = await handle.createWritable();
  await writable.write(json);
  await writable.close();

/*
  // ファイルに保存 (古典的方法。保存先を選択できない)
  const a = document.createElement('a');
  a.href = 'data:text/plain,' + encodeURIComponent(json);
  a.download = 'keymap.json';
  a.style.display = 'none';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
*/
});

/********** キーマップのUI処理 **********/

// キーマップ → UI表示
function showKeyMap()
{
  // レイヤー番号
  text_layer.textContent = `レイヤー ${layerNum+1}/8`;

  // 使用する
  check_enabled.checked  = keyMaps[layerNum].Enabled;
  // 名前
  text_name.value = keyMaps[layerNum].Name;
  // LED表示色
  color_led.value = keyMaps[layerNum].Led;

  for(let key = 0; key < KEY_MAX; key++){
    // キー修飾
    const modifiers = keyMaps[layerNum].Modifiers[key];
    check_alt  [key].checked = ((modifiers & ALT_MASK  ) != 0);
    check_ctrl [key].checked = ((modifiers & CTRL_MASK ) != 0);
    check_shift[key].checked = ((modifiers & SHIFT_MASK) != 0);
    // キー割当
    text_key[key].value = getKeyName(keyMaps[layerNum].KeyCodes[key]);
  }
}

// UI表示 → キーマップ
function setKeyMap()
{
  // 使用する
  keyMaps[layerNum].Enabled = check_enabled.checked;
  // 名前
  keyMaps[layerNum].Name = text_name.value;
  // LED表示色
  keyMaps[layerNum].Led = color_led.value;

  for(let key = 0; key < KEY_MAX; key++){
    // キー修飾
    let modifiers = 0;
    if(check_alt  [key].checked) modifiers |= ALT_MASK;
    if(check_ctrl [key].checked) modifiers |= CTRL_MASK;
    if(check_shift[key].checked) modifiers |= SHIFT_MASK;
    keyMaps[layerNum].Modifiers[key] = modifiers;
    // キー割当
    keyMaps[layerNum].KeyCodes[key] = getKeyCode(text_key[key].value);
  }
}

/********** キーマップのシリアル通信処理 ***********/

// キーマップのロード（デバイスから読み出し）
async function loadKeyMap()
{
  let keyMapsTemp = new Array(); 
  let success = true;
  let errorLayer = -1;

  // レイヤーごとにREADコマンドを実行
  for(let layer = 0; layer < PAGE_MAX; layer++){
    // コマンド送信
    const command = COM_READ;
    const data = [ layer ];
    await sendCommand(command, data);
    // 応答受信
    const telegram = await recvResponse();
    if(telegram != false){
      // コマンドのチェック
      const command = telegram[3];
      if(command != COM_READ){
        switch(command){
          case ERR_CHECKSUM:
            error_toast("ERROR: デバイス側でチェックサムエラーがありました。");
            success = false; errorLayer = layer;
            break;
          case ERR_COMMAND:
            error_toast("ERROR: デバイス側で不正なコマンドと判定されました。");
            success = false; errorLayer = layer;
            break;
          case ERR_PARAM:
            error_toast("ERROR: デバイス側で不正なパラメータと判定されました。");
            success = false; errorLayer = layer;
          break;
            default:
            error_toast("ERROR: 未定義のエラーを受信しました。");
            success = false; errorLayer = layer;
            break;
        }
      }
      // レイヤー番号のチェック
      const recvLayer    = telegram[4];
      if(layer != recvLayer){
        error_toast("ERROR: レイヤー番号が一致しません。 " + layer + " / " + recvLayer);
        success = false; errorLayer = layer;
        break;
      }
      // 受信データの解釈
      const bData = telegram.slice(5);
      const keyMap = BytesToKeyMap(bData);
      if(keyMap == false){
        success = false; errorLayer = layer;
        break;
      }
      keyMapsTemp[layer] = keyMap;
    }else{
      // 応答受信失敗
      success = false; errorLayer = layer;
      break;
    }
  }
  // 全レイヤーのREADが成功したらデータ更新
  if(success){
    // キーマップデータを更新
    keyMaps = keyMapsTemp;
    // 表示を更新
    layerNum = 0;
    showKeyMap();

    show_toast("キーマップ情報を読み出しました。");
    return true;
  }else{
    error_toast("ERROR: レイヤー " + errorLayer + " でエラーがありました。処理を中断します。");
    return false;
  }
}

// キーマップのセーブ (デバイスへ書き込み)
async function saveKeyMap()
{
  let success = true;
  let errorLayer = -1;

  // キーマップデータを更新
  setKeyMap();

  // レイヤーごとにWRITEコマンドを実行
  for(let layer = 0; layer < PAGE_MAX; layer++){
    // 送信データの生成
    const bData = KeyMapToBytes(keyMaps[layer]);
    const data = new Uint8Array(45)
    data[0] = layer;
    for(let i = 0; i < 44; i++) data[1 + i] = bData[i];
    // コマンド送信
    const command = COM_WRITE;
    await sendCommand(command, data);
    // 応答受信
    const telegram = await recvResponse();
    if(telegram != false){
      // 応答のチェック
      const response = telegram[3];
      if(response != COM_WRITE){
        switch(response){
          case ERR_CHECKSUM:
            error_toast("ERROR: デバイス側でチェックサムエラーがありました。");
            success = false; errorLayer = layer;
            break;
          case ERR_COMMAND:
            error_toast("ERROR: デバイス側で不正なコマンドと判定されました。");
            success = false; errorLayer = layer;
            break;
          case ERR_PARAM:
            error_toast("ERROR: デバイス側で不正なパラメータと判定されました。");
            success = false; errorLayer = layer;
            break;
          default:
            error_toast("ERROR: 未定義のエラーを受信しました。");
            success = false; errorLayer = layer;
            break;
        }
      }
      // レイヤー番号のチェック
      const recvLayer    = telegram[4];
      if(layer != recvLayer){
        error_toast("ERROR: レイヤー番号が一致しません。 " + layer + " / " + recvLayer);
        success = false; errorLayer = layer;
        break;
      }
    }else{
      // 応答受信失敗
      success = false; errorLayer = layer;
      break;
    }
  }
  if(success){
    show_toast("キーマップ情報を書き込みました。");
  }else{
    error_toast("ERROR: レイヤー " + errorLayer + " でエラーがありました。処理を中断します。");
  }
}

// 配列をゼロ終端までに切り詰める
function BytesTrim(data)
{
  let end = 0;
  for(end = 0; end < 20; end++){
    if(data[end] == 0x00) break;
  }
  if (end == 0) return new Uint8Array(0);
  return data.slice(0,end);
}

// バイト列データ → キーマップ
function BytesToKeyMap(data)
{
  // 使用する？
  const bEnabled = data[0];
  const enabled = (bEnabled == 0) ? false : true;
  // 名前
  const bName = BytesTrim(data.slice(1,21));
  const name = new TextDecoder().decode(bName);
  // LED表示色
  const r = ("00" + data[21].toString(16)).slice( -2 );
  const g = ("00" + data[22].toString(16)).slice( -2 );
  const b = ("00" + data[23].toString(16)).slice( -2 );
  const led = "#" + r + g + b;
  // 各キーについて
  let modifiers = new Array();
  let keyCodes  = new Array();    
  for(let key = 0; key < KEY_MAX; key++){
    modifiers[key] = data[24 + 2 * key];
    keyCodes[key]  = data[25 + 2 * key];
  }
  const keyMap = new KeyMap(enabled, name, led, modifiers, keyCodes);
  return keyMap;
}

// キーマップ → バイト列データ
function KeyMapToBytes(keyMap)
{
  const data = new Uint8Array(44);

  // 使用する？
  data[0] = keyMap.Enabled ? 0x01 : 0x00;
  // 名前
  const bName = new TextEncoder().encode(keyMap.Name);
  for(let i = 0; i < bName.length; i++){
    data[1 + i] = bName[i];
  }
  // LED表示色
  data[21] = parseInt(keyMap.Led.substring(1, 3), 16);
  data[22] = parseInt(keyMap.Led.substring(3, 5), 16);
  data[23] = parseInt(keyMap.Led.substring(5, 7), 16);
  // 各キーについて
  for(let key = 0; key < KEY_MAX; key++){
    data[24 + 2 * key] = keyMap.Modifiers[key];
    data[25 + 2 * key] = keyMap.KeyCodes[key];
  }
  return data;
}

/********** シリアル通信処理 (コマンド送信と応答受信) ***********/

// コマンドの送信
async function sendCommand(command, data)
{
  // 電文の生成
  const telegram =  new Uint8Array(data.length + 5);
  telegram[0] = 0xAA;
  telegram[1] = 0x55;
  telegram[2] = data.length + 2;
  telegram[3] = command;
  //    データ
  for (let i = 0; i < data.length; i++) {
    telegram[4 + i] = data[i];
  }
  //    チェックサム
  let sum = 0;
  for (let i = 0; i < telegram.length - 1; i++) {
    sum += telegram[i];
  }
  telegram[4 + data.length] = sum & 0xFF;

  // 電文の送信
  const writer = comPort.writable.getWriter();
  await writer.write(telegram);
  writer.releaseLock();
}

// コマンド応答の受信
async function recvResponse()
{
  let isError  = false; // エラーか？
  let received = false; // 電文受信成功したか？
  let recvCnt = 0;      // 電文の受信バイトカウント
  let len = 0;          // 電文の Len の値
  const telegram =  new Uint8Array(256); // 電文受信バッファ

  // COMポートが受信可能か？
  if (comPort.readable) {
    const reader = comPort.readable.getReader();
    try {
      // 電文受信完了かエラーまで
      while (true) {
        // COMポートからデータ受信 (タイムアウトあり)
        value = await read_with_timeout(reader, 1000);
        if (value == false) {
          isError = true; // タイムアウト
          break;
        }
        //console.log("Received " + value);
        
        // 受信したデータを1バイトずつ処理
        for(let i = 0; i < value.length; i++){
          const bData = value[i];
          telegram[recvCnt] = bData;
          
          // 1バイト目: AA
          if(recvCnt == 0){
            if(bData != 0xAA){
              isError = true;
              error_toast("ERROR: Header 1 not AA but " + bData.toString(16));
            }
          }
          // 2バイト目: 55
          else if(recvCnt == 1){
            if(bData != 0x55){
              isError = true;
              error_toast("ERROR: Header 1 not 55 but " + bData.toString(16));
            }
          }
          // 3バイト目: Len
          else if(recvCnt == 2){
            len = bData;
          }
          // 最後のバイト
          else if(recvCnt == len + 2){
            // チェックサム検証
            let sum = 0;
            for(let j = 0; j < len + 2; j++){ sum += telegram[j]; }
            sum = sum & 0xFF;
            if(sum != bData){
              isError = true;
              error_toast("ERROR: Check Sum " + sum.toString(16) + "!= " + bData.toString(16));
            }else{
              received = true;
            }
          }
          if (isError) break;
          if (received) break;
          recvCnt++;
        }
        if (isError) break;
        if (received) break;
      }
    } catch (error) {
      error_toast("ERROR: " + error);
    } finally {
      reader.releaseLock();
    }
  }else{
    error_toast("ERROR: COM Port not readable");
  }

  // 電文受信完了したか？
  if(received){
    return telegram; // 成功なら電文を返す
  }else{
    return false;    // 失敗ならfalseを返す
  }
}

// タイムアウト付きシリアル受信
async function read_with_timeout(reader, timeout)
{
  let reading = true; // 受信中フラグ

  // シリアル受信のプロミス
  const receivePromise = new Promise(async(resolve, reject) => {
    const { value, done } = await reader.read();
    reading = false; // 受信終了
    if (done) {
      reject("ERROR: No Data or Port Closed");
    } else{
      resolve(value);
    }
  });
  // タイムアウトのプロミス
  const timeoutPromise = new Promise((resolve, reject) => {
    setTimeout(() => {
      // readerをキャンセルすることでシリアル受信のプロミスも終わらせる
      if(reading) reader.cancel();
      reject("ERROR: Timeout")
    }, timeout);
  });
  // シリアル受信かタイムアウトかどちらかが終わるのを待つ
  try{
    const buff = await Promise.race([receivePromise, timeoutPromise]);
    return buff; // 受信成功
  } catch(error) {
    error_toast("ERROR: " + error);
    return false; // 受信失敗
  }
}

// トースト表示
function show_toast(message)
{
  console.log(message);

  const jsFrame = new JSFrame();
  jsFrame.showToast({
    html: message, align: 'top', duration: 3000
  });
}

// トースト表示(エラー)
function error_toast(message)
{
  console.log(message);

  const jsFrame = new JSFrame();
  jsFrame.showToast({
    html: message, align: 'top', duration: 5000
  });
}
